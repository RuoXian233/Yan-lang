#include "yan-lang.hpp"


class Translator final {
public:
    explicit Translator(NodeBase *ast) : ast(ast) {}

    std::string Visit(NodeBase *node) {
        switch (node->nodeType) {
        case NodeType::Expression:
            return this->VisitExpression(node);
        case NodeType::Number:
            return this->VisitNumber(node);
        case NodeType::SingleExpression:
            return this->VisitSingleExpression(node);
        case NodeType::VarAccess:
            return this->VisitVarAccessNode(node);
        case NodeType::VarAssign:
            return this->VisitVarAssignNode(node);
        case NodeType::IfExpression:
            return this->VisitIfExpressionNode(node);
        case NodeType::ForExpression:
            return this->VisitForExpression(node);
        case NodeType::WhileExpression:
            return this->VisitWhileExpression(node);
        case NodeType::FunctionDefinition:
            return this->VisitFunctionDefinition(node);
        case NodeType::FunctionCall:
            return this->VisitFunctionCall(node);
        case NodeType::String:
            return this->VisitString(node);
        case NodeType::List:
            return this->VisitList(node);
        case NodeType::Return:
            return this->VisitReturn(node);
        case NodeType::Continue:
            return this->VisitContinue(node);
        case NodeType::Break:
            return this->VisitBreak(node);
        case NodeType::Subscription:
            return this->VisitSubscription(node);
        case NodeType::Dictionary:
            return this->VisitDictionary(node);
        case NodeType::Attribution:
            return this->VisitAttribution(node);
        case NodeType::AdvancedVarAccess:
            return this->VisitAdvancedVarAccess(node);
        case NodeType::NewExpression:
            return this->VisitNewExpression(node);
        case NodeType::AttributionCall:
            return this->VisitAttributionCall(node);
        case NodeType::NonlocalStatement:
            return this->VisitNonlocal(node);
        case NodeType::Defer:
            return this->VisitDefer(node);
        }
        return "";
    }

    std::string VisitExpression(NodeBase *node) {
        auto exprNode = dynamic_cast<BinaryOperationNode *>(node);
        std::string opValue {};
        switch (exprNode->binaryOperator.type) {
        case TokenType::OP_Plus:
            opValue = "+";
            break;
        case TokenType::OP_Minus:
            opValue = "-";
            break;
        case TokenType::OP_Mul:
            opValue = "*";
            break;
        case TokenType::OP_Div:
            opValue = "/";
            break;
        case TokenType::OP_Pow:
            opValue = "**";
            break;
        case TokenType::OP_Equal:
            opValue = "==";
            break;
        case TokenType::OP_Nequal:
            opValue = "!=";
            break;
        case TokenType::OP_Gt:
            opValue = ">";
            break;
        case TokenType::OP_Lt:
            opValue = "<";
            break;
        case TokenType::OP_Gte: 
            opValue = ">="; 
            break;
        case TokenType::OP_Lte:
            opValue = "<=";
            break;  
        case TokenType::Keyword:
            if (exprNode->binaryOperator.Matches<std::string>(TokenType::Keyword, std::string("and"))) {
                opValue = "and";
            } else if (exprNode->binaryOperator.Matches<std::string>(TokenType::Keyword, std::string("or"))) {
                opValue = "or";
            } else {
                std::cerr << "Unsupported binary operator: " << *(std::string *) exprNode->binaryOperator.value << std::endl;
                assert(false);
            }
            break;
        default:
            std::cerr << "Unsupported binary operator: " << static_cast<int>(exprNode->binaryOperator.type) << std::endl;
            assert(false);
        }
        return std::format("{} {} {}", Visit(exprNode->left), opValue, Visit(exprNode->right));
    }

    std::string VisitSingleExpression(NodeBase *node) {
        auto uNode = dynamic_cast<UnaryOperationNode *>(node);
        std::string opValue {};
        switch (uNode->unaryOperator.type) {
        case TokenType::OP_Plus:
            opValue = "+";
            break;

        case TokenType::Keyword:
            opValue = "not ";
            break;
        default:
            std::cerr << "Unsupported unary operator: " << uNode->unaryOperator.value << std::endl;
            assert(false);
        }

        return std::format("{}{}", opValue, Visit(uNode->node));
    }

    std::string VisitVarAccessNode(NodeBase *node) {
        return *(std::string *) dynamic_cast<VariableAccessNode *>(node)->variableNameToken.value;
    }

    std::string VisitVarAssignNode(NodeBase *node) {
        auto varAssignNode = dynamic_cast<VariableAssignNode *>(node);
        return std::format("{} = {}", *(std::string *) varAssignNode->variableNameToken.value, Visit(varAssignNode->valueNode));
    }

    inline std::string GetIndent(int indentCount) {
        std::stringstream ss;
        for (int i = 0; i < indentCount; i++) {
            ss << TAB;
        }
        return ss.str();
    }

    std::string VisitNumber(NodeBase *node) {
        auto numNode = dynamic_cast<NumberNode *>(node);
        if (numNode->numberToken.type == TokenType::Int) {
            return std::format("{}", *(int *) numNode->numberToken.value);
        } else {
            return std::format("{}", *(double *) numNode->numberToken.value);
        }
    }

    std::string VisitIfExpressionNode(NodeBase *node) {
        auto ifNode = dynamic_cast<IfExpressionNode *>(node);
        std::string result {};
        indent++;

        result += "if " + Visit(ifNode->cases[0].first.first) + ":\n";
        result += Visit(ifNode->cases[0].first.second);

        for (int i = 1; i < ifNode->cases.size(); i++) {
            auto caseNode = ifNode->cases[i];
            result += GetIndent(indent - 1) + "elif " + Visit(caseNode.first.first) + ":\n";
            result += Visit(caseNode.first.second);
        }

        if (ifNode->elseCase.second) {
            result += GetIndent(indent - 1) + "else:\n";
            result += Visit(ifNode->elseCase.first);
        }

        indent--;
        return result;
    }
    
    std::string VisitForExpression(NodeBase *node) {
        auto forExpressionNode = dynamic_cast<ForExpressionNode *>(node);
        std::string result {};
        indent++;

        if (forExpressionNode->rangeBasedLoop) {
            result += "for " + *((std::string *) forExpressionNode->var.value) + " in " + Visit(forExpressionNode->range) + ":\n";
        } else {
            if (forExpressionNode->stepvNode) {
                result += "for " + *((std::string *) forExpressionNode->var.value) + " in range(" + Visit(forExpressionNode->stvNode) + ", " + Visit(forExpressionNode->etvNode) + ", " + Visit(forExpressionNode->stepvNode) + "):\n";
            } else {
                result += "for " + *((std::string *) forExpressionNode->var.value) + " in range(" + Visit(forExpressionNode->stvNode) + ", " + Visit(forExpressionNode->etvNode) + "):\n";
            }
        }
        result += Visit(forExpressionNode->body);
        indent--;
        return result;
    }
    
    std::string VisitWhileExpression(NodeBase *node) {
        auto whileNode = dynamic_cast<WhileExpressionNode *>(node);
        std::string result {};
        indent++;

        result += + "while " + Visit(whileNode->conditionNode) + ":\n";
        result += Visit(whileNode->body);
        indent--;
        return result;
    }
    
    std::string VisitFunctionCall(NodeBase *node) {
        auto funcCallNode = dynamic_cast<FunctionCallNode *>(node);
        std::string result {};
        auto t = Visit(funcCallNode->target);

        if (Lexer::Contains(builtinNames, t)) {
            result += "yan_builtin_impl_";
        }

        result += t;
        result += "(";

        int index = 0;
        for (auto arg : funcCallNode->arguments) {
            result += Visit(arg);
            if (index != funcCallNode->arguments.size() - 1) {
                result += ", ";
            }
            index++;
        }
        result += ")";
        return result;
    }

    std::string VisitFunctionDefinition(NodeBase *node) {
        auto funcDefNode = dynamic_cast<FunctionDefinitionNode *>(node);
        std::string result {};
        indent++;
        std::string funcName {};
        if (!funcDefNode->fun.value) {
            funcName = "__yan_anonymous_func__";
        } else {
            funcName = *(std::string *) funcDefNode->fun.value;
        }
        result += "def " + funcName + "(";

        int index = 0;
        for (auto arg : funcDefNode->parameters) {
            auto argName = *(std::string *) arg.value;
            if (argName.starts_with("_") && argName.ends_with("_")) {
                result += '*' + argName.substr(1, argName.size() - 2);
            } else {
                result += argName;
            }
            if (index != funcDefNode->parameters.size() - 1) {
                result += ", ";
            }
            index++;
        }
        result += "):\n";
        result += Visit(funcDefNode->body);
        indent--;
        return result;
    }
    
    std::string VisitString(NodeBase *node) {
        auto stringNode = dynamic_cast<StringNode *>(node);
        std::stringstream ss;
        (new String(*(std::string *) stringNode->stringToken.value))->Representation(ss);
        return ss.str();
    }

    std::string VisitList(NodeBase *node) {
        std::string result {};
        auto listNode = dynamic_cast<ListNode *>(node);
        if (!listNode->isStatements) {
            if (dynamic_cast<ListNode *>(node)->elements.empty()) {
                return "[]";
            }

            int index = 0;
            int len = dynamic_cast<ListNode *>(node)->elements.size();
            for (auto item : dynamic_cast<ListNode *>(node)->elements) {
                result += Visit(item);
                if (index != len - 1) {
                    result += ", ";
                }
                index++;
            }
            return "[" +  result + "]";
        } else {
            for (auto item : dynamic_cast<ListNode *>(node)->elements) {
                result += GetIndent(indent) + Visit(item) + "\n";
            }
            return result;
        }
    }

    std::string VisitReturn(NodeBase *node) {
        auto returnNode = dynamic_cast<ReturnStatementNode *>(node);
        if (returnNode->nodeToReturn) {
            return std::format("return {}", Visit(returnNode->nodeToReturn));
        } else {
            return "return";
        }
    }

    std::string VisitContinue(NodeBase *node) {
        return "continue";
    }

    std::string VisitBreak(NodeBase *node) {
        return "break";
    }

    std::string VisitSubscription(NodeBase *node) {
        auto subscriptionNode = dynamic_cast<SubscriptionNode *>(node);
        std::string result {};
        result += Visit(subscriptionNode->target) + "[" + Visit(subscriptionNode->index) + "]";
        for (auto item : subscriptionNode->subIndexes) {
            result += "[" + Visit(item) + "]";
        }
        if (subscriptionNode->assignment) {
            result += " = " + Visit(subscriptionNode->assignment);
        }
        return result;
    }

    std::string VisitDictionary(NodeBase *node) {
        auto dictNode = dynamic_cast<DictionaryNode *>(node);
        std::string result = "__yan_dict_impl({ ";

        int count = 1;
        for (const auto &[k, v] : dictNode->elements) {
            if (v->nodeType == NodeType::FunctionDefinition) {
                std::string inlineFunc = "lambda ";
                for (auto arg : dynamic_cast<FunctionDefinitionNode *>(v)->parameters) {
                    auto argName = *(std::string *) arg.value;
                    if (argName.starts_with("_") && argName.ends_with("_")) {
                        inlineFunc += '*' + argName.substr(1, argName.size() - 2);
                    } else {
                        inlineFunc += argName;
                    }
                }   
                inlineFunc += ": " + Visit(dynamic_cast<FunctionDefinitionNode *>(v)->body);
                result += Visit(k) + ": " + inlineFunc;
            } else {
                result += Visit(k) + ": " + Visit(v);
            }

            if (count != dictNode->elements.size()) {
                result += ", ";
            }
            count++;
        }

        result += " })";
        return result;
    }

    std::string VisitAttribution(NodeBase *node) {
        auto attributionNode = dynamic_cast<AttributionNode *>(node);
        std::string result {};
        if (attributionNode->calls.find(0) != attributionNode->calls.end()) {
            result += Visit(attributionNode->target) + "." + Visit(attributionNode->calls[0]);
        } else {
            result += Visit(attributionNode->target) + "." + *(std::string *) attributionNode->attr.value;
        }

        int index = 1;
        for (auto item : attributionNode->subAttrs) {
            if (attributionNode->calls.find(index) != attributionNode->calls.end()) {
                result += "." + Visit(attributionNode->calls[index]);
            } else {
                result += "." + *(std::string *) item.value;
            }
            index++;
        }

        if (attributionNode->assignment) {
            result += " = " + Visit(attributionNode->assignment);
        }
        return result;
    }

    std::string VisitAdvancedVarAccess(NodeBase *node) {
        auto advancedVarAccessNode = dynamic_cast<AdvancedVarAccessNode *>(node);
        std::string result {};
        for (auto item : advancedVarAccessNode->advancedAccess) {
            result += Visit(item);
        }
        return result;
    }
    
    std::string VisitNewExpression(NodeBase *node) {
        auto newNode = dynamic_cast<NewExprNode *>(node);
        if (newNode->newExpr->nodeType == NodeType::FunctionCall) {
            return std::format("{}", Visit(dynamic_cast<NewExprNode *>(node)->newExpr));
        } else {
            return std::format("{}()", Visit(dynamic_cast<NewExprNode *>(node)->newExpr));
        }
    }

    std::string VisitAttributionCall(NodeBase *node) {
        auto attributionCallNode = dynamic_cast<AttributionCallNode *>(node);
        std::string result {};
        result += Visit(attributionCallNode->call);
        return result;
    }

    std::string VisitNonlocal(NodeBase *node) {
        return std::format("nonlocal {}", *(std::string *) dynamic_cast<NonlocalStatementNode *>(node)->freeVar.value);
    }

    std::string VisitDefer(NodeBase *node) {
        return std::format("__yan_keyword_impl_defer('{}')", Visit(dynamic_cast<DeferNode *>(node)->deferExpr));
    }

    std::string ToPython() {
        return Visit(ast);
    }

    ~Translator() = default;

private:
    NodeBase *ast;
    int indent = 0;
    static constexpr auto TAB = "    ";
};
