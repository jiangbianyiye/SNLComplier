#pragma once
#include"Scanner.h"

#include<list>
#include<string>
#include<map>
#include<Token.h>

struct ParseTreeNode;
using Terminal = Tag;
enum class Nonterminal;

class Parser
{
private:
	Scanner& scanner;
public:
	ParseTreeNode * getParseTree();
	void printParseTree(ParseTreeNode * parseTree);
	Parser(Scanner& scan);
	~Parser();

private:
	Token curToken;
	void readNextToken();
	void match(Terminal expectedTer);
	void syntaxError(const char* errMessage);

	ParseTreeNode * program();
		ParseTreeNode * programHead();


		ParseTreeNode * declarePart();

			ParseTreeNode * typeDec();
				ParseTreeNode * typeDeclaration();
					ParseTreeNode * typeDecList();
						void typeId(ParseTreeNode * typeDecNode);
						void typeName(ParseTreeNode * typeDecNode);
							void baseType(ParseTreeNode * typeDecNode);
							void structureType(ParseTreeNode * typeDecNode);
								void arrayType(ParseTreeNode * typeDecNode);
								void recType(ParseTreeNode * typeDecNode);
									ParseTreeNode * fieldDecList();
										ParseTreeNode * fieldDecMore();
										void idList(ParseTreeNode * typeDecNode);
											void idMore(ParseTreeNode * typeDecNode);
						ParseTreeNode * typeDecMore();

			ParseTreeNode * varDec();
				ParseTreeNode * varDeclaration();
					ParseTreeNode *varDecList();
						ParseTreeNode * varDecMore(); 
						void varIdList(ParseTreeNode * varDecNode);
							void varIdMore(ParseTreeNode * varDecNode);

			ParseTreeNode * procDec();
				ParseTreeNode * procDeclaration();
					void paramList(ParseTreeNode * procDecNode);
						ParseTreeNode * paramDecList();
							ParseTreeNode * paramMore();
							ParseTreeNode * param();
								void formList(ParseTreeNode * paramDecNode);
									void fidMore(ParseTreeNode * paramDecNode);
						
					ParseTreeNode * procDecPart();

					ParseTreeNode * procBody();
				

		ParseTreeNode * programBody();
			ParseTreeNode * stmList();
				ParseTreeNode * stmMore();
				ParseTreeNode * stm();
					ParseTreeNode * assCall();
						ParseTreeNode * callStmRest();
							ParseTreeNode * actParamList();
								ParseTreeNode * actParamMore();
					ParseTreeNode * assignmentRest();
					ParseTreeNode * conditionalStm();
					ParseTreeNode * loopStm();
					ParseTreeNode * inputStm();
					ParseTreeNode * outputStm();
					ParseTreeNode * returnStm();

					ParseTreeNode * exp();
						ParseTreeNode * simpleExp();
							ParseTreeNode * term();
								ParseTreeNode * factor();
									ParseTreeNode * variable();
										void variMore(ParseTreeNode * varNode);
											ParseTreeNode * fieldvar();
												void fieldvarMore(ParseTreeNode * varNode);
						

};

enum class Nonterminal {
	Program, ProgramHead, ProgramName, DeclarePart,
	TypeDec, TypeDeclaration, TypeDecList, TypeDecMore,
	TypeId, TypeName, BaseType, StructureType,
	ArrayType, Low, Top, RecType,
	FieldDecList, FieldDecMore, IdList, IdMore,
	VarDec, VarDeclaration, VarDecList, VarDecMore,
	VarIdList, VarIdMore, ProcDec, ProcDeclaration,
	ProcDecMore, ProcName, ParamList, ParamDecList,
	ParamMore, Param, FormList, FidMore,
	ProcDecPart, ProcBody, ProgramBody, StmList,
	StmMore, Stm, AssCall, AssignmentRest,
	ConditionalStm, StmL, LoopStm, InputStm,
	InVar, OutputStm, ReturnStm, CallStmRest,
	ActParamList, ActParamMore, RelExp, OtherRelE,
	Exp, OtherTerm, Term, OtherFactor,
	Factor, Variable, VariMore, FieldVar,
	FieldVarMore, CmpOp, AddOp, MultOp
};

enum class ParseNodeType {
	PROG, PHEAD, DECPART, TYPEDEC, VARDEC, PROCDEC, STMLIST, STMT, EXP
};

enum class DecType {
	ARRAY, CHAR, INTEGER, RECORD, ID
};

enum class StmtType {
	IF, WHILE, ASSIGN, READ, WRITE, CALL, RETURN
};

enum class ExpType {
	OP, CONST, VAR
};

enum class VariableType {
	ID, ARRAYMEMB, FIELDMEMB
};

enum class CheckType {
	VOID, INTEGER, BOOLEAN
};

enum class ParamType {
	VALPARAM, VARPARAM
};

#define MAXCHILDRENNUM 3

struct IdAttr;
using SymbolTable = std::map<std::string, IdAttr>;

struct ParseTreeNode
{
public:
	ParseTreeNode(){
		for (int i = 0; i < MAXCHILDRENNUM; ++i) {
			this->children[i] = nullptr;
		}
		this->sibling = nullptr;
		idNum = 0;
	}

	ParseTreeNode(ParseNodeType nodeType):
		ParseTreeNode()
	{
		this->nodeType = nodeType;
	}

	ParseTreeNode(ParseNodeType nodeType, DecType decType) :
		ParseTreeNode(nodeType)
	{
		this->specificType.dec = decType;
	}

	ParseTreeNode(ParseNodeType nodeType, StmtType stmtType):
		ParseTreeNode(nodeType)
	{
		this->specificType.stmt = stmtType;
	}

	ParseTreeNode(ParseNodeType nodeType, ExpType expType) :
		ParseTreeNode(nodeType)
	{
		this->specificType.exp = expType;

		this->attr.exp.varType = VariableType::ID;
		this->attr.exp.checkType = CheckType::VOID;
	}

	~ParseTreeNode() {
		for (int i = 0; i < MAXCHILDRENNUM; ++i) {
			delete this->children[i];
		}
		delete this->sibling;
	}
public:
	ParseTreeNode* children[MAXCHILDRENNUM];
	ParseTreeNode* sibling;
	//int lineNo;

	ParseNodeType nodeType;
	union SpecificType {
		DecType dec;
		StmtType stmt;
		ExpType exp;
	} specificType;

	int idNum;
	std::string name[10];
	SymbolTable symbolTable;
	union Attr{
		Attr() {};
		~Attr() {};
		struct{
			unsigned int lowerBound;
			unsigned int upperBound;
			DecType type;
		}array;

		struct {
			ParamType paramType;
		}process;

		struct{
			Tag op;
			int value;
			VariableType varType;
			CheckType checkType;
		}exp;

		std::string typeName;
	}attr;
};

/******************************************************
**********���������Ҫ�õ������ͼ���������************
******************************************************/
/*��ʶ��������*/
enum class IdKind {TYPE, VAR, PROC};
/*���������dir��ֱ�ӱ���(ֵ��)��indir��ʾ��ӱ���(���)*/
enum class AccessKind {DIR, INDIR };

/*�βα�Ľṹ����*/
struct  ParamTable
{
	SymbolTable   *  entry;/*ָ����β����ڷ��ű��еĵ�ַ���*/
	ParamTable  *  next;
};

/*��ʶ�������Խṹ����*/
struct IdAttr {
	struct typeIR  * idtype;		/*ָ���ʶ���������ڲ���ʾ*/
	IdKind    kind;					/*��ʶ��������*/
	union
	{
		struct
		{
			AccessKind   access;   /*�ж��Ǳ�λ���ֵ��*/
			int          level;
			int          off;
			bool         isParam;  /*�ж��ǲ���������ͨ����*/

		}VarAttr;/*������ʶ��������*/
		struct
		{
			int         level;     /*�ù��̵Ĳ���*/

			ParamTable  * param;   /*������*/

			int         mOff;	   /*���̻��¼�Ĵ�С*/

			int         nOff;  	   /*sp��display���ƫ����*/

			int         procEntry; /*���̵���ڵ�ַ*/

			int         codeEntry;/*������ڱ��,�����м��������*/

		}ProcAttr;/*��������ʶ��������*/

	}specific;/*��ʶ���Ĳ�ͬ�����в�ͬ������*/
};

enum class TypeKind{ intTy, charTy, arrayTy, recordTy, boolTy };
struct FieldChain
{
	std::string  id;             /*������*/
	int    off;                 /*���ڼ�¼�е�ƫ��*/
	typeIR  *  UnitType; /*���г�Ա������*/
	FieldChain  * Next;
};

struct  typeIR
{
	int				size;   /*������ռ�ռ��С*/
	TypeKind		kind;
	union
	{
		struct
		{
			struct typeIR	* indexTy;
			struct typeIR	* elemTy;
			int    low;     /*��¼�������͵��½�*/
			int    up;      /*��¼�������͵��Ͻ�*/
		}ArrayAttr;
		FieldChain * body;  /*��¼�����е�����*/
	} More;
};