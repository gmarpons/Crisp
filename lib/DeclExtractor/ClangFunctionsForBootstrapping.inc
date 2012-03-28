pl_get_one(declContext, Decl, const DeclContext*, getDeclContext)
pl_get_one(nameAsString, NamedDecl, std::string, getNameAsString)
pl_get_one(numParams, FunctionDecl, unsigned, getNumParams)
pl_get_one(resultType, FunctionDecl, QualType, getResultType)
pl_get_one(asString, QualType, std::string, getAsString)
pl_get_one(access, Decl, enum AccessSpecifier, getAccess)
pl_get_one(size_overridden_methods, CXXMethodDecl, unsigned, size_overridden_methods)

pl_check_property(definition, CXXRecordDecl, hasDefinition)

pl_get_many(base, CXXRecordDecl, CXXRecordDecl::base_class_const_iterator, CXXRecordDecl::bases_begin, CXXRecordDecl::bases_end, bases_it)
pl_get_many(method, CXXRecordDecl, CXXRecordDecl::method_iterator, CXXRecordDecl::method_begin, CXXRecordDecl::method_end, methods_it)
pl_get_many(overridden_methods, CXXMethodDecl, CXXMethodDecl::method_iterator, CXXMethodDecl::begin_overridden_methods, CXXMethodDecl::end_overridden_methods, overridden_methods_it)
