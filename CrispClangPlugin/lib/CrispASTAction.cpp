#include <string>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

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
      FactsOutputStream = new llvm::raw_fd_ostream("-", ErrorInfo);
    }

    virtual ~CrispConsumer() {
      delete FactsOutputStream;
    }
    
    llvm::raw_ostream &facts() {
      return *FactsOutputStream;
    }
    
    virtual void HandleTranslationUnit(ASTContext &Context) {
      facts() << "Handling translation unit!\n";

      // traverse AST to visit declarations and statements
      TraverseDecl(Context.getTranslationUnitDecl());
      facts() << "Traversing of the AST done!\n";
    }
    
    // Visit declarations
    
    virtual bool VisitDecl(Decl *D) {
      SourceManager &SM = CompilerInstance.getSourceManager();
      SourceLocation SL = D->getLocation();
      unsigned int L = SM.getSpellingLineNumber(SL);
      unsigned int C = SM.getSpellingColumnNumber(SL);
      const char *FN = SM.getBufferName(SL);

      facts() << "Decl: " << FN << ":" << L <<  ":" << C << "\n";
      return true;
    }

    // Visit types
    
    virtual bool VisitType(Type *T) {
      facts() << "Type: " << T->getTypeClassName() << "\n";
      return true;
    }

  private:
    CompilerInstance &CompilerInstance;
    llvm::raw_ostream *FactsOutputStream;
    std::string ErrorInfo;
    std::string RulesFileName;
  };
  
  class CrispASTAction : public PluginASTAction {
  public:
    CrispASTAction() {
    }
    
  protected:
    virtual ASTConsumer* CreateASTConsumer(CompilerInstance& CI
                                           , llvm::StringRef) {
      return new CrispConsumer(CI, RulesFileName);
    }
    
    virtual bool ParseArgs(const CompilerInstance &CI
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

  private:
    std::string RulesFileName;
  };
}

static FrontendPluginRegistry::Add<CrispASTAction>
X("crisp-clang", "Data extraction clang plugin for CRISP");
