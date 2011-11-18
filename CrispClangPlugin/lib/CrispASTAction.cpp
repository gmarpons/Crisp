#include <string>
#include <vector>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "PrologEngine.h"

using namespace llvm;
using namespace clang;
using namespace prolog;
namespace b = boost;
namespace l = boost::lambda;

namespace {

  class CrispConsumer : public ASTConsumer
                      , public RecursiveASTVisitor<CrispConsumer> {
  public:
    CrispConsumer(CompilerInstance &CI, std::string &RFN)
      : CompilerInstance(CI)
      , ErrorInfo()
      , RulesFileName(RFN) {
      // ErrorInfo is an output arg to get info about potential errors
      // opening the file/stream.
      FactsOutputStream = new raw_fd_ostream("-", ErrorInfo);
    }
    virtual ~CrispConsumer();
    virtual void HandleTranslationUnit(ASTContext &Context);
    virtual bool VisitDecl(Decl *D);
    virtual bool VisitCXXMethodDecl(Decl *D);
    virtual bool VisitType(Type *T);

  private:
    raw_ostream &facts();
    void VisitTypeFromTypesTable(Type *T);

  private:
    CompilerInstance &CompilerInstance;
    raw_ostream *FactsOutputStream;
    std::string ErrorInfo;
    std::string RulesFileName;
  };

}

CrispConsumer::~CrispConsumer() {
  delete FactsOutputStream;
}
    
raw_ostream & CrispConsumer::facts() {
  return *FactsOutputStream;
}
    
void CrispConsumer::VisitTypeFromTypesTable(Type *T) {
  if (isa<PointerType>(T)) {
    std::string Sort("PointerType");
    (void) plAssertTypeIsA(T, Sort); // Return value ignored
  }
}

void CrispConsumer::HandleTranslationUnit(ASTContext &Context) {
  DEBUG(dbgs() << "Handling translation unit!\n");
  int Success = plRunEngine(RulesFileName);
  
  if (Success) {
    // Traverse types in the translation unit
    b::for_each(b::make_iterator_range(Context.types_begin()
                                       , Context.types_end())
                , l::bind(&CrispConsumer::VisitTypeFromTypesTable
                          , this
                          , l::_1)
                );
    
    // traverse AST to visit declarations and statements
    TraverseDecl(Context.getTranslationUnitDecl());
    DEBUG(dbgs() << "Traversing of the AST done!\n");
    
    // when debugging, open a PROLOG interactive session
    DEBUG(Success = plInteractiveSession());
  }
  
  DEBUG(if (Success) dbgs() << "Translation unit analyzed.\n";
        else dbgs() << "Analysis aborted: Prolog engine failed.\n";);
  (void) plCleanUp(Success ? 0 : 1); // Return value ignored
}
    
// Visit declarations

// TODO: handle Prolog errors when visiting AST (now return values of
// pl* funcs are ignored).
    
bool CrispConsumer::VisitDecl(Decl *D) {
  SourceManager &SM = CompilerInstance.getSourceManager();
  SourceLocation SL = D->getLocation();
  unsigned int L = SM.getSpellingLineNumber(SL);
  unsigned int C = SM.getSpellingColumnNumber(SL);
  const char *FN = SM.getBufferName(SL);
  
  DEBUG(dbgs() << "Decl: " << FN << ":" << L <<  ":" << C << "\n");
  return true;
}

bool CrispConsumer::VisitCXXMethodDecl(Decl *D) {
  std::string Sort("CXXMethodDecl");
  (void) plAssertDeclIsA(D, Sort); // Return value ignored
  return true;
}

// Visit types
    
bool CrispConsumer::VisitType(Type *T) {
  DEBUG(dbgs() << "Type: " << T->getTypeClassName() << "\n");
  return true;
}

namespace {

  class CrispASTAction : public PluginASTAction {
  public:
    CrispASTAction() {
      DebugFlag = 1;            // FIXME: use a plugin option to
                                // activate debug mode.
    }
    
  protected:
    virtual ASTConsumer* CreateASTConsumer(CompilerInstance& CI
                                           , StringRef) {
      return new CrispConsumer(CI, RulesFileName);
    }
    
    virtual bool ParseArgs(const CompilerInstance &CI
                           , const std::vector<std::string> &Args);
  private:
    std::string RulesFileName;
  };
}

bool CrispASTAction::ParseArgs(const CompilerInstance &CI
                               , const std::vector<std::string> &Args) {
  // One argument needed: rules file name.
  if (Args.size() != 1) {
    DiagnosticsEngine &DE = CI.getDiagnostics();
    std::string DiagMsg = "rules file missing";
    unsigned DiagId = DE.getCustomDiagID(DiagnosticsEngine::Error, DiagMsg);
    DE.Report(DiagId);
    return false;
  }
  RulesFileName = Args[0];
  return true;
}

static FrontendPluginRegistry::Add<CrispASTAction>
X("crisp-clang", "Data extraction clang plugin for CRISP");
