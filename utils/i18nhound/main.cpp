#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;

class FindRawStringLiteralVisitor
    : public RecursiveASTVisitor<FindRawStringLiteralVisitor> {
public:
  explicit FindRawStringLiteralVisitor(
      ASTContext *Context, const std::vector<std::string> &MacroNames,
      const std::vector<llvm::StringRef> &SourcePaths,
      CompilerInstance &Compiler)
      : Context(Context), MacroNames(MacroNames), SourcePaths(SourcePaths),
        Compiler(Compiler) {}

  bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *FD = CE->getDirectCallee()) {
      for (const auto &Name : MacroNames) {
        if (Name == FD->getNameAsString()) {
          for (unsigned i = 0, e = CE->getNumArgs(); i != e; ++i) {
            if (auto *SL =
                    dyn_cast<StringLiteral>(CE->getArg(i)->IgnoreImpCasts())) {
              // Found a string literal argument of the named function.
              SkipLiteralIDs.insert(SL->getID(*Context));
            }
          }
        }
      }
    }

    return true;
  }

  bool VisitStringLiteral(StringLiteral *SL) {
    SourceManager &SourceManager = Context->getSourceManager();
    SourceLocation Loc = SL->getBeginLoc();
    FullSourceLoc FullLoc(Loc, SourceManager);

    if (SkipLiteralIDs.contains(SL->getID(*Context))) {
      return true;
    }

    if (SourceManager.getFileID(FullLoc.getSpellingLoc()) !=
        SourceManager.getMainFileID()) {
      return true;
    }

    if ((SourceManager.isMacroArgExpansion(Loc) ||
         SourceManager.isMacroBodyExpansion(Loc))) {
      auto ExpandedMacroName = Lexer::getImmediateMacroName(
          Loc, SourceManager, Context->getLangOpts());
      for (const auto &MacroName : MacroNames) {
        if (MacroName == ExpandedMacroName) {
          return true;
        }
      }
    }

    auto &DE = Context->getDiagnostics();
    auto DiagID = DE.getCustomDiagID(DiagnosticsEngine::Warning,
                                     "raw string literal found");
    auto Report = DE.Report(Loc, DiagID);
    // Report.setForceEmit();

    return true;
  }

private:
  ASTContext *Context;
  std::vector<std::string> MacroNames;
  std::vector<llvm::StringRef> SourcePaths;
  CompilerInstance &Compiler;
  std::set<int64_t> SkipLiteralIDs;
};

class FindRawStringLiteralConsumer : public clang::ASTConsumer {
public:
  explicit FindRawStringLiteralConsumer(
      ASTContext *Context, const std::vector<std::string> &MacroNames,
      const std::vector<llvm::StringRef> &SourcePaths,
      CompilerInstance &Compiler)
      : Visitor(Context, MacroNames, SourcePaths, Compiler) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindRawStringLiteralVisitor Visitor;
};

class FindRawStringLiteralAction : public clang::ASTFrontendAction {
public:
  FindRawStringLiteralAction(const std::vector<std::string> &MacroNames,
                             const std::vector<llvm::StringRef> &SourcePaths)
      : MacroNames(MacroNames), SourcePaths(SourcePaths) {}

  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<FindRawStringLiteralConsumer>(
        &Compiler.getASTContext(), MacroNames, SourcePaths, Compiler);
  }

private:
  std::vector<std::string> MacroNames;
  std::vector<llvm::StringRef> SourcePaths;
};

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("i18nhound options");

class MyFrontendActionFactory : public FrontendActionFactory {
public:
  MyFrontendActionFactory(std::vector<std::string> MacroNames,
                          std::vector<llvm::StringRef> SourcePaths)
      : MacroNames(MacroNames), SourcePaths(SourcePaths) {}

  std::unique_ptr<FrontendAction> create() override {
    // Now we can pass data to the MyFrontendAction constructor.
    return std::make_unique<FindRawStringLiteralAction>(MacroNames,
                                                        SourcePaths);
  }

private:
  std::vector<std::string> MacroNames;
  std::vector<llvm::StringRef> SourcePaths;
};

int main(int argc, const char **argv) {
  std::vector<std::string> MacroNames;
  MacroNames.push_back("_");
  MacroNames.push_back("N_");
  MacroNames.push_back("gettext");

  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  std::vector<llvm::StringRef> SourcePaths;
  for (auto &Source : Tool.getSourcePaths()) {
    auto File = Tool.getFiles().getFile(Source);
    SourcePaths.push_back(Tool.getFiles().getCanonicalName(File.get()));
  }

  return Tool.run(new MyFrontendActionFactory(MacroNames, SourcePaths));
}
