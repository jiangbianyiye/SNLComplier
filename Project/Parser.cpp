#include "Parser.h"
#include<iostream>

static std::string TEMP_NAME;

ParseTreeNode* Parser::getParseTree() {
	ParseTreeNode* parseTreeRoot = nullptr;
	readNextToken();
	parseTreeRoot = program();
	if (curToken.tag != Tag::ENDFILE)
		syntaxError("Code ends before file\n");
	return 	 parseTreeRoot;
}

void Parser::readNextToken() {
	readNextToken();
}

void Parser::match(Terminal expectedTer) {
	if (curToken.tag == expectedTer) {
		readNextToken();
	}
	else {
		syntaxError("can not match Terminal");
		//TO-DO ����ִ��
		exit(0);
	}
}

void Parser::syntaxError(const char* errMessage) {
	std::cerr << "error: syntax error at line" << curToken.line << ": " << errMessage << std::endl;
}

/*����ʽ: program --> programHead declarePart programBody .  */
ParseTreeNode * Parser::program() {
	ParseTreeNode * progHead = programHead();
	ParseTreeNode * declPart = declarePart();
	ParseTreeNode * progBody = programBody();

	ParseTreeNode * root = new(std::nothrow) ParseTreeNode(ParseNodeType::PROG);
	if (root != nullptr) {
		if (progHead != nullptr) 
			root->children[0] = progHead;
		else 
			syntaxError("a program head is expected!");

		root->children[1] = declPart;

		if (progBody != nullptr)
			root->children[2] = progBody;
		else 
			syntaxError("a program body is expected!");
	}
	match(Tag::DOT);
	return root;
}

/* ����ʽ programHead --> PROGRAM  ProgramName            */
ParseTreeNode * Parser::programHead() {
	ParseTreeNode * pheadNode = new(std::nothrow) ParseTreeNode(ParseNodeType::PHEAD);

	match(Terminal::PROGRAM);
	if ((pheadNode != nullptr) && (curToken.tag == Tag::ID)) {
		pheadNode->name = curToken.lexeme;
	}
	match(Terminal::ID);
	return 	 pheadNode;
}


/* ����ʽ declarePart --> typeDec  varDec  procDec              */
ParseTreeNode * Parser::declarePart() {
	ParseTreeNode * typeDecNode = new(std::nothrow) ParseTreeNode(ParseNodeType::TYPEDEC);
	if (typeDecNode != nullptr) {
		ParseTreeNode * typeDeclare = typeDec();
		if (typeDeclare != nullptr) {
			typeDecNode->children[0] = typeDeclare;
		}
		else {
			delete typeDecNode;
			typeDecNode = nullptr;
		}
	}

	ParseTreeNode *	varDecNode = new(std::nothrow) ParseTreeNode(ParseNodeType::VARDEC);
	if (varDecNode != nullptr) {
		ParseTreeNode * varDeclare = varDec();
		if (varDeclare != nullptr) {
			varDecNode->children[0] = varDeclare;
		}
		else {
			delete varDeclare;
			varDecNode = nullptr;
		}
	}
	ParseTreeNode * procDecNode = procDec();
	
	if (varDecNode == nullptr) {
		varDecNode = procDecNode;
	}

	if (typeDecNode == nullptr) {
		typeDecNode = varDecNode;
	}

	if (typeDecNode != varDecNode) {
		typeDecNode->sibling = varDecNode;
	}
	
	if (varDecNode != procDecNode) {
		varDecNode->sibling = procDecNode;
	}

	return 	typeDecNode;
}

/* ����ʽ  typeDec --> �� | TypeDeclaration                      */
ParseTreeNode * Parser::typeDec() {
	ParseTreeNode * typeDecNode = nullptr;
	switch (curToken.tag) {
	case Tag::TYPE:
		typeDecNode = typeDeclaration();
		break;
	case Tag::VAR:
	case Tag::PROCEDURE:
	case Tag::BEGIN:
		break;
	default:
		readNextToken();
		syntaxError("unexpected token!");
	}
	return 	typeDecNode;
}

/* ����ʽ TypeDeclaration --> TYPE  TypeDecList                 */
ParseTreeNode * Parser::typeDeclaration() {
	match(Terminal::TYPE);
	ParseTreeNode * node = typeDecList();
	if (node == nullptr) {
		syntaxError("a type declaration is expected!");
	}
	return node;
}

/* TypeDecList --> typeId = typeName ; typeDecMore       */
ParseTreeNode * Parser::typeDecList() {
	ParseTreeNode * typeDecListNode = new(std::nothrow) ParseTreeNode(ParseNodeType::DECPART);
	if (typeDecListNode != nullptr) {
		typeId(typeDecListNode);
		match(Terminal::EQ);
		typeName(typeDecListNode);
		match(Terminal::SEMI);
		ParseTreeNode * typeDecMoreNode = typeDecMore();
		if (typeDecMoreNode != nullptr) {
			typeDecListNode->sibling = typeDecMoreNode;
		}
	}
	return typeDecListNode;
}

/* ����ʽ typeDecMore -->   �� | TypeDecList                   */
ParseTreeNode * Parser::typeDecMore() {
	ParseTreeNode * typeDecMoreNode = nullptr;
	switch (curToken.tag) {
	case Tag::VAR:
	case Tag::PROCEDURE:
	case Tag::BEGIN:
		break;
	case Tag::ID:
		typeDecMoreNode = typeDecList();
	default:
		readNextToken();
		syntaxError("unexpected token !");
		break;
	}
	return 	 typeDecMoreNode;
}

/* ����ʽ  typeId  -->  id                              */
void Parser::typeId(ParseTreeNode * typeDecNode)
{
	int& idnum = (typeDecNode->sameIdNum);
	if ((curToken.tag == Tag::ID))
	{
		typeDecNode->name = curToken.lexeme;
		idnum += 1;
	}
	match(Terminal::ID);
}

/* ����ʽ  typeName -->  baseType | structureType | id    */
void Parser::typeName(ParseTreeNode * typeDecNode) {
	switch (curToken.tag)
	{
	case Tag::INTEGER:
	case Tag::CHAR:
		baseType(typeDecNode);
		break;
	case Tag::ARRAY:
	case Tag::RECORD:
		structureType(typeDecNode);
		break;
	case Tag::ID:
		typeDecNode->specificType.dec = DecType::ID;
		typeDecNode->attr.typeName = curToken.lexeme;
		match(Terminal::ID);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}

/* ����ʽ  baseType -->  INTEGER | CHAR                 */
void Parser::baseType(ParseTreeNode * typeDecNode) {
	switch (curToken.tag)
	{
	case Tag::INTEGER:
		match(Terminal::INTEGER);
		typeDecNode->specificType.dec = DecType::INTEGER;
		break;
	case Tag::CHAR:
		match(Terminal::CHAR);
		typeDecNode->specificType.dec = DecType::CHAR;
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}

/* ����ʽ  structureType -->  arrayType | recType      */
void Parser::structureType(ParseTreeNode * typeDecNode) {
	switch (curToken.tag)
	{
	case Tag::ARRAY:
		arrayType(typeDecNode);
		break;
	case Tag::RECORD:
		typeDecNode->specificType.dec = DecType::RECORD;  //try handle it in recType?
		recType(typeDecNode);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}

/* ����ʽ  arrayType -->  ARRAY [low..top] OF baseType       */
void Parser::arrayType(ParseTreeNode * typeDecNode) {
	match(Terminal::ARRAY);
	match(Terminal::LMIDPAREN);

	if (curToken.tag == Tag::INTC) {
		typeDecNode->attr.array.lowerBound = std::stoi(curToken.lexeme);
	}

	match(Terminal::INTC);
	match(Terminal::UNDERANGE);

	if (curToken.tag == Tag::INTC) {
		typeDecNode->attr.array.upperBound= std::stoi(curToken.lexeme);
	}

	match(Terminal::INTC);
	match(Terminal::RMIDPAREN);
	match(Terminal::OF);

	baseType(typeDecNode);
	typeDecNode->attr.array.type = typeDecNode->specificType.dec;
	typeDecNode->specificType.dec = DecType::ARRAY;
}

/*   recType -->  RECORD fieldDecList END                  */
void Parser::recType(ParseTreeNode * typeDecNode) {
	match(Terminal::RECORD);
	ParseTreeNode * recNode = fieldDecList();
	if (recNode != nullptr) {
		typeDecNode->children[0] = recNode;
	}
	else {
		syntaxError("a record body is requested!");
	}
	match(Terminal::END);
}

/* ����ʽ    fieldDecList -->  baseType idList ; fieldDecMore      */
/*                            | arrayType idList; fieldDecMore     */
ParseTreeNode * Parser::fieldDecList() {
	ParseTreeNode * typeDecNode = new(std::nothrow) ParseTreeNode(ParseNodeType::DECPART);
	ParseTreeNode * fieldDecNode = nullptr;

	if (typeDecNode != nullptr) {
		switch (curToken.tag)
		{
		case Tag::INTEGER:
		case Tag::CHAR:
			baseType(typeDecNode);
			break;
		case Tag::ARRAY:
			arrayType(typeDecNode);
			break;
		default:
			readNextToken();
			syntaxError("unexpected token here!");
			break;
		}
		idList(typeDecNode);
		match(Terminal::SEMI);
		fieldDecNode = fieldDecMore();
		typeDecNode->sibling = fieldDecNode;
	}
	return typeDecNode;
}

/* ����ʽ   fieldDecMore -->  �� | fieldDecList          */
ParseTreeNode * Parser::fieldDecMore() {
	ParseTreeNode *  fieldDecNode = nullptr;
	switch (curToken.tag)
	{
	case Tag::END:
		break;
	case Tag::INTEGER:
	case Tag::CHAR:
	case Tag::ARRAY:
		fieldDecNode = fieldDecList();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return fieldDecNode;
}

/* ����ʽ idList -->  id  idMore      */
void Parser::idList(ParseTreeNode * typeDecNode) {
	if (curToken.tag == Tag::ID) {
		typeDecNode->name = curToken.lexeme;
		match(Terminal::ID);
		typeDecNode->sameIdNum += 1;
	}
	idMore(typeDecNode);
}

/* ����ʽ  idMore --> �� |  , idList         */
void Parser::idMore(ParseTreeNode * typeDecNode) {
	switch (curToken.tag)
	{
	case Tag::SEMI:
		break;
	case Tag::COMMA:
		match(Terminal::COMMA);
		idList(typeDecNode);
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}

/* ����ʽ  varDec --> �� |  varDeclaration               */
ParseTreeNode *  Parser::varDec() {
	ParseTreeNode * varDecNode = nullptr;
	switch (curToken.tag) {
	case Tag::PROCEDURE:
	case Tag::BEGIN:
		break;
	case Tag::VAR:
		varDecNode = varDeclaration();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token");
		break;
	}
	return varDecNode;
}


/* ����ʽ  varDeclaration --> VAR  varDecList          */
ParseTreeNode * Parser::varDeclaration() {
	match(Terminal::VAR);
	ParseTreeNode * node = varDecList();
	if (node == nullptr) {
		syntaxError("a var declaration is expected!");
	}
	return node;
}

/* ����ʽ  varDecList -->  typeName varIdList; varDecMore        */
ParseTreeNode * Parser::varDecList() {
	ParseTreeNode * varDecNode = new(std::nothrow) 	ParseTreeNode(ParseNodeType::DECPART);

	if (varDecNode != nullptr) {
		typeName(varDecNode);
		varIdList(varDecNode);
	}
	match(Terminal::SEMI);
	varDecNode->sibling = varDecMore();

	return varDecNode;
}

/* ����ʽ  varDecMore  -->  �� |  varDecList               */
ParseTreeNode * Parser::varDecMore() {
	ParseTreeNode * varDecNode = nullptr;
	switch (curToken.tag)
	{
	case Tag::PROCEDURE:
	case Tag::BEGIN:
		break;
	case Tag::INTEGER:
	case Tag::CHAR:
	case Tag::ARRAY:
	case Tag::RECORD:
	case Tag::ID:
		varDecNode = varDecList();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return varDecNode;
}

/* ����ʽ  varIdList -->  id  varIdMore               */
void Parser::varIdList(ParseTreeNode * varDecNode) {
	if (curToken.tag == Tag::ID) {
		varDecNode->name = curToken.lexeme;
		match(Terminal::ID);
		varDecNode->sameIdNum += 1;
	}							   
	else {
		readNextToken();
		syntaxError("a varid is expected here!");
	}
	varIdMore(varDecNode);
}

/* ����ʽ  varIdMore -->  �� |  , varIdList                      */
void Parser::varIdMore(ParseTreeNode * varDecNode) {
	switch (curToken.tag)
	{
	case Tag::SEMI:
		break;
	case Tag::COMMA:
		match(Terminal::COMMA);
		varIdList(varDecNode);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}


/* ����ʽ  procDec --> �� |  procDeclaration                    */
ParseTreeNode * Parser::procDec() {
	ParseTreeNode  * procDecNode = nullptr;
	switch (curToken.tag)
	{					
	case Tag::BEGIN:
		break;
	case Tag::PROCEDURE:
		procDecNode = procDeclaration();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return procDecNode;
}


/* ����ʽ  procDeclaration    -->  PROCEDURE                        */
/*                                 ProcName(paramList);             */
/*                                 procDecPart                      */
/*                                 procBody                         */
/*                                 procDec                          */
ParseTreeNode * Parser::procDeclaration() {
	ParseTreeNode  * procDecNode = new(std::nothrow) ParseTreeNode(ParseNodeType::PROCDEC);
	match(Terminal::PROCEDURE);
	if (procDecNode != nullptr) {
		if (curToken.tag == Tag::ID) {
			procDecNode->name = curToken.lexeme;
			procDecNode->sameIdNum += 1;
			match(Terminal::ID);
		}

		match(Terminal::LPAREN);
		paramList(procDecNode);
		match(Terminal::RPAREN);
		match(Terminal::SEMI);

		procDecNode->children[1] = procDecPart();
		procDecNode->children[2] = procBody();
		procDecNode->sibling = procDec();
	}
	return 	 procDecNode;
}

/* ����ʽ   paramList -->  �� |  paramDecList                     */
void Parser::paramList(ParseTreeNode * procDecNode) {
	ParseTreeNode * formalParam = nullptr;
	switch (curToken.tag)
	{
	case Tag::RPAREN:
		break;
	case Tag::INTEGER:
	case Tag::CHAR:
	case Tag::ARRAY:
	case Tag::RECORD:
	case Tag::ID:
	case Tag::VAR:
		formalParam = paramDecList();
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	procDecNode->children[0] = formalParam;
}

/* ����ʽ  paramDecList --> param  paramMore        */
ParseTreeNode * Parser::paramDecList() {
	ParseTreeNode * formalparam = param();
	ParseTreeNode * moreParam = paramMore();
	if (formalparam != nullptr) {
		formalparam->sibling = moreParam;
	}
	return formalparam;
}

/* ����ʽ  paramMore -->  �� | ; paramDecList                     */
ParseTreeNode * Parser::paramMore() {
	ParseTreeNode * formalparam = nullptr;
	switch (curToken.tag)
	{
	case Tag::RPAREN:
		break;
	case Tag::SEMI:
		match(Terminal::SEMI);
		formalparam = paramDecList();
		if (formalparam == nullptr) {
			syntaxError("a param declaration is request!");
		}
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return formalparam;
}

/* ����ʽ  param -->  typeName formList | VAR typeName formList  */
ParseTreeNode * Parser::param() {
	ParseTreeNode * paramDecNode = new(std::nothrow) ParseTreeNode(ParseNodeType::DECPART);
	if (paramDecNode != nullptr) {
		switch (curToken.tag)
		{
		case Tag::INTEGER:
		case Tag::CHAR:
		case Tag::ARRAY:
		case Tag::RECORD:
		case Tag::ID:
			paramDecNode->attr.process.paramType = ParamType::VALPARAM;
			typeName(paramDecNode);
			formList(paramDecNode);
			break;
		case Tag::VAR:
			match(Terminal::VAR);
			paramDecNode->attr.process.paramType = ParamType::VARPARAM;
			typeName(paramDecNode);
			formList(paramDecNode);
			break;
		default:
			readNextToken();
			syntaxError("unexpected token here!");
			break;
		}
	}
	return paramDecNode;
}

/* ����ʽ  formList -->  id  fidMore     */
void Parser::formList(ParseTreeNode * paramDecNode) {
	if (curToken.tag == Tag::ID) {
		paramDecNode->name = curToken.lexeme;
		paramDecNode->sameIdNum += 1;
		match(Terminal::ID);
	}
	fidMore(paramDecNode);
}

/* ����ʽ  fidMore  -->   �� |  , formList       */
void Parser::fidMore(ParseTreeNode * paramDecNode) {
	switch (curToken.tag)
	{
	case Tag::SEMI:
	case Tag::RPAREN:
		break;
	case Tag::COMMA:
		match(Terminal::COMMA);
		formList(paramDecNode);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}

/* ����ʽ  procDecPart -->  declarePart        */
ParseTreeNode * Parser::procDecPart() {
	return declarePart();
}


/* ����ʽ  procBody -->  programBody                             */
ParseTreeNode *  Parser::procBody() {
	ParseTreeNode * progBodyNode = programBody();
	if (progBodyNode == nullptr) {
		syntaxError("a program body is requested!");
	}
	return progBodyNode;
}

/* ����ʽ  programBody --> BEGIN  stmList   END     */
ParseTreeNode *  Parser::programBody() {
	ParseTreeNode * progBody = new(std::nothrow) ParseTreeNode(ParseNodeType::STMLIST);
	match(Terminal::BEGIN);
	if (progBody != nullptr) {
		progBody->children[0] = stmList();
	}
	match(Terminal::END);
	return 	 progBody;
}


/* ����ʽ  stmList -->  stm    stmMore        */
ParseTreeNode * Parser::stmList()
{
	ParseTreeNode * stmNode = stm();
	ParseTreeNode * stmMoreNode = stmMore();
	if (stmNode != nullptr) {
		stmNode->sibling = stmMoreNode;
	}
	return stmNode;
}


/* ��䲿�ֵĴ�����	                    	        	*/
/* ����ʽ < stmMore > -->   �� |  ; stmList                 */
ParseTreeNode * Parser::stmMore()
{
	ParseTreeNode * stmListNode = NULL;
	switch (curToken.tag)
	{
	case Tag::ELSE:
	case Tag::FI:
	case Tag::END:
	case Tag::ENDWH:	break;
	case Tag::SEMI:
		match(Terminal::SEMI);
		stmListNode = stmList();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return stmListNode;
}




/* ����ʽ   stm   -->   conditionalStm   {IF}                       */
/*                    | loopStm          {WHILE}                    */
/*                    | inputStm         {READ}                     */
/*                    | outputStm        {WRITE}                    */
/*                    | returnStm        {RETURN}                   */
/*                    | id  assCall      {id}                       */
ParseTreeNode * Parser::stm()
{
	ParseTreeNode * stmNode = nullptr;
	switch (curToken.tag)
	{
	case Tag::IF:
		stmNode = conditionalStm();
		break;
	case Tag::WHILE:
		stmNode = loopStm();
		break;
	case Tag::READ:
		stmNode = inputStm();
		break;
	case Tag::WRITE:
		stmNode = outputStm();
		break;
	case Tag::RETURN:
		stmNode = returnStm();
		break;
	case Tag::ID:
		TEMP_NAME = curToken.lexeme;
		match(Terminal::ID);
		stmNode = assCall();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return stmNode;
}


/* ��䲿�ֵĴ�����	                         	        	*/
/* ����ʽ   assCall  -->   assignmentRest   {:=,LMIDPAREN,DOT}  */
/*                        | callStmRest      {(}                */
ParseTreeNode * Parser::assCall()
{
	ParseTreeNode * assCallNode = nullptr;
	switch (curToken.tag)
	{
	case Tag::ASSIGN:
	case Tag::LMIDPAREN:
	case Tag::DOT:
		assCallNode = assignmentRest();
		break;
	case Tag::LPAREN:
		assCallNode = callStmRest();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return assCallNode;
}


/* ��ֵ��䲿�ֵĴ�����	                    	      */
/* ����ʽ   assignmentRest  -->  variMore :=  exp         */
ParseTreeNode * Parser::assignmentRest()
{
	ParseTreeNode * assiStmtNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT,StmtType::ASSIGN);

	if (assiStmtNode != nullptr) {
		ParseTreeNode * variMoreNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::VAR);
		if (variMoreNode != nullptr) {
			variMoreNode->name = TEMP_NAME;
			variMoreNode->sameIdNum += 1;
			variMore(variMoreNode);
		}
		assiStmtNode->children[0] = variMoreNode;
		match(Terminal::ASSIGN);
		assiStmtNode->children[1] = exp();
	}
	return assiStmtNode;
}


/* ������䲿�ֵĴ�����	                    	               */
/* ����ʽ   conditionalStm --> IF exp THEN stmList ELSE stmList FI */
ParseTreeNode * Parser::conditionalStm()
{
	ParseTreeNode * condStmNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::IF);

	match(Terminal::IF);
	if (condStmNode != nullptr) {
		condStmNode->children[0] = exp();
	}
	match(Terminal::THEN);
	if (condStmNode != nullptr) {
		condStmNode->children[1] = stmList();
	}

	if (curToken.tag == Tag::ELSE)
	{
		match(Terminal::ELSE);
		if (condStmNode != NULL) {
			condStmNode->children[2] = stmList();
		}
	}
	match(Tag::FI);
	return condStmNode;
}


/* ѭ����䲿�ֵĴ�����	                    	        */
/* ����ʽ  loopStm -->  WHILE exp DO stmList ENDWH          */
ParseTreeNode * Parser::loopStm()
{
	ParseTreeNode * loopStmNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::WHILE);

	match(Terminal::WHILE);
	if (loopStmNode != nullptr) {
		loopStmNode->children[0] = exp();
	}
	match(Terminal::DO);
	if (loopStmNode != nullptr) {
		loopStmNode->children[1] = stmList();
	}
	match(Terminal::ENDWH);

	return loopStmNode;
}


/* ������䲿�ֵĴ�����	                       */
/* ����ʽ   inputStm  -->   READ(id)             */
ParseTreeNode * Parser::inputStm()
{
	ParseTreeNode * inputStmNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::READ);
	
	match(Terminal::READ);
	match(Terminal::LPAREN);
	if (inputStmNode != nullptr && curToken.tag == Tag::ID) {
		inputStmNode->name = curToken.lexeme;
		inputStmNode->sameIdNum += 1;
	}
	match(Terminal::ID);
	match(Terminal::RPAREN);

	return inputStmNode;
}


/* �����䲿�ֵĴ�����	                    	  */
/* ����ʽ   outputStm   -->   WRITE(exp)              */
ParseTreeNode * Parser::outputStm()
{
	ParseTreeNode *	outputStmNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::WRITE);

	match(Terminal::WRITE);
	match(Terminal::LPAREN);
	if (outputStmNode != nullptr) {
		outputStmNode->children[0] = exp();
	}
	match(Terminal::RPAREN);

	return nullptr;
}


/* ������䲿�ֵĴ�����	                        */
/* ����ʽ   returnStm  -->   RETURN                 */
ParseTreeNode * Parser::returnStm()
{
	ParseTreeNode *	returnStmNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::RETURN);

	match(Terminal::RETURN);

	return returnStmNode;
}


/* ����������䲿�ֵĴ�����	                  	   */
/* ����ʽ   callStmRest  -->  (actParamList)           */
ParseTreeNode * Parser::callStmRest()
{
	ParseTreeNode *	callStmRestNode = new(std::nothrow) ParseTreeNode(ParseNodeType::STMT, StmtType::CALL);

	match(Terminal::LPAREN);
	if (callStmRestNode != nullptr) {
		ParseTreeNode * paramNode = new(std::nothrow)  ParseTreeNode(ParseNodeType::EXP,ExpType::VAR);
		if (paramNode != nullptr) {
			paramNode->name = TEMP_NAME;
			paramNode->sameIdNum += 1;
		}
		paramNode->children[0] = paramNode;
		paramNode->children[1] = actParamList();
	}
	match(Terminal::RPAREN);

	return callStmRestNode;
}


/* ��������ʵ�β��ֵĴ�����	                	        */
/* ����ʽ   actParamList  -->  �� |  exp actParamMore       */
ParseTreeNode * Parser::actParamList()
{
	ParseTreeNode * expNode = nullptr;

	switch (curToken.tag)
	{
	case Tag::RPAREN:
		break;
	case Tag::ID:
	case Tag::INTC:
		expNode = exp();
		if (expNode != nullptr) {
			expNode->sibling = actParamMore();
		}
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}

	return expNode;
}


/* ��������ʵ�β��ֵĴ�����	                	        */
/* ����ʽ   actParamMore  -->  �� |  , actParamList         */
ParseTreeNode * Parser::actParamMore()
{
	ParseTreeNode * actParamListNode = nullptr;

	switch (curToken.tag)
	{
	case Tag::RPAREN:
		break;
	case Tag::COMMA:
		match(Terminal::COMMA);
		actParamListNode = actParamList();
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
	return actParamListNode;
}

	
/* ���ʽ������													 */
/* ����ʽ < ���ʽ > -->  < �򵥱��ʽ > [< ��ϵ����� > < �򵥱��ʽ > ]	*/
ParseTreeNode * Parser::exp()
{
	ParseTreeNode * simpleExpNode = simpleExp();

	if (curToken.tag == Tag::LT || curToken.tag == Tag::EQ) {
		ParseTreeNode * relationOptNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::OP);
		if (relationOptNode != nullptr) {
			relationOptNode->children[0] = simpleExpNode;
			relationOptNode->attr.exp.op = curToken.tag;

			simpleExpNode = relationOptNode;
		}
		match(curToken.tag);
		if (simpleExpNode != nullptr) {
			simpleExpNode->children[1] = simpleExp();
		}
	}

	return simpleExpNode;
}

/* �򵥱��ʽ������											*/
/* ����ʽ < �򵥱��ʽ >  -->	< �� > { < �ӷ������ > < �� > }			*/
ParseTreeNode * Parser::simpleExp()
{
	ParseTreeNode * termNode = term();
	
	while (curToken.tag == Tag::PLUS || curToken.tag == Tag::MINUS) {
		ParseTreeNode * expNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::OP);
		if (expNode != nullptr) {
			expNode->children[0] = termNode;
			expNode->attr.exp.op = curToken.tag;

			termNode = expNode;

			match(curToken.tag);

			termNode->children[1] = term();
		}
	}

	return termNode;
}


/* �����													*/
/* ����ʽ < �� > ::= < ���� > { < �˷������ > < ���� > }		*/
ParseTreeNode * Parser::term()
{
	ParseTreeNode * factorNode = factor();

	while (curToken.tag == Tag::TIMES || curToken.tag == Tag::OVER) {
		ParseTreeNode * expNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::OP);
		if (expNode != nullptr) {
			expNode->children[0] = factorNode;
			expNode->attr.exp.op = curToken.tag;

			factorNode = expNode;

			match(curToken.tag);

			factorNode->children[1] = factor();
		}
	}

	return factorNode;
}


/* ���Ӵ�����											    */
/* ����ʽ factor ::= ( exp ) | INTC | variable       		*/
ParseTreeNode * Parser::factor()
{
	ParseTreeNode * factorNode = nullptr;

	switch (curToken.tag)
	{
	case Tag::INTC:
		factorNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP,ExpType::CONST);
		if (factorNode != nullptr) {
			factorNode->attr.exp.value = std::stoi(curToken.lexeme);
		}
		match(Terminal::INTC);
		break;
	case Tag::ID:
		factorNode = variable();
		break;
	case Tag::LPAREN:
		match(Terminal::LPAREN);
		factorNode = exp();
		match(Terminal::RPAREN);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}

	return factorNode;
}


/* ����������										*/
/* ����ʽ variable   -->   id variMore          	*/
ParseTreeNode * Parser::variable() {
	ParseTreeNode * varNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::VAR);

	if (varNode !=nullptr) {
		varNode->name = curToken.lexeme;
		varNode->sameIdNum += 1;
	}

	match(Terminal::ID);
	variMore(varNode);
	return varNode;
}


/* ����������												*/
/* ����ʽ variMore   -->  ��                             	*/
/*                       | [exp]            {[}             */
/*                       | . fieldvar       {DOT}           */
void Parser::variMore(ParseTreeNode * varNode)
{
	switch (curToken.tag)
	{
	case Tag::ASSIGN:
	case Tag::TIMES:
	case Tag::EQ:
	case Tag::LT:
	case Tag::PLUS:
	case Tag::MINUS:
	case Tag::OVER:
	case Tag::RPAREN:
	case Tag::RMIDPAREN:
	case Tag::SEMI:
	case Tag::COMMA:
	case Tag::THEN:
	case Tag::ELSE:
	case Tag::FI:
	case Tag::DO:
	case Tag::ENDWH:
	case Tag::END:
		break;
	case Tag::LMIDPAREN:
		match(Terminal::LMIDPAREN);
		varNode->children[0] = exp();
		varNode->attr.exp.varType = VariableType::ID;
		varNode->children[0]->attr.exp.varType = VariableType::ID;
		match(Tag::RMIDPAREN);
		break;
	case Tag::DOT:
		match(Terminal::DOT);
		varNode->children[0] = fieldvar();
		varNode->attr.exp.varType = VariableType::FIELDMEMB;
		varNode->children[0]->attr.exp.varType = VariableType::ID;
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}


/* ����������												*/
/* ����ʽ fieldvar   ::=  id  fieldvarMore                          */
ParseTreeNode * Parser::fieldvar()
{
	ParseTreeNode * varNode = new(std::nothrow) ParseTreeNode(ParseNodeType::EXP, ExpType::VAR);

	if (varNode != nullptr) {
		varNode->name = curToken.lexeme;
		varNode->sameIdNum += 1;
	}
	match(Terminal::ID);
	fieldvarMore(varNode);

	return nullptr;
}


/* ����������												*/
/* ����ʽ fieldvarMore   ::=  ��                             		*/
/*                           | [exp]            {[}                 */
void Parser::fieldvarMore(ParseTreeNode * varNode)
{
	switch (curToken.tag)
	{
	case Tag::ASSIGN:
	case Tag::TIMES:
	case Tag::EQ:
	case Tag::LT:
	case Tag::PLUS:
	case Tag::MINUS:
	case Tag::OVER:
	case Tag::RPAREN:
	case Tag::SEMI:
	case Tag::COMMA:
	case Tag::THEN:
	case Tag::ELSE:
	case Tag::FI:
	case Tag::DO:
	case Tag::ENDWH:
	case Tag::END:
		break;
	case Tag::LMIDPAREN:
		match(Tag::LMIDPAREN);
		varNode->children[0] = exp();
		varNode->children[0]->attr.exp.varType = VariableType::ARRAYMEMB;
		match(Terminal::RMIDPAREN);
		break;
	default:
		readNextToken();
		syntaxError("unexpected token here!");
		break;
	}
}


Parser::Parser(Scanner& scan):
	scanner(scan)
{
}

Parser::~Parser()
{
}
