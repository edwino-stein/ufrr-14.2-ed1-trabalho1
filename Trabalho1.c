/*
	PROGRAMA PRINCIPAL

	Edwino Stein - 1201324411 
*/

#include "dependencies/common/common.h"

#include "dependencies/header/console.h"
#include "dependencies/header/stack.h"
#include "dependencies/header/html.h"
#include "dependencies/header/messages.h"
#include "cmdsPrototype.h"

#define LINUX_INVOCATION "sensible-browser > /dev/null %s &"
#define OSX_INVOCATION "open %s"

/*
	Imprime a tela de ajuda
*/
void printHelpScreen(cmdList * cmds);

/*
	Transforma uma pilha de comandos em um array javascript ao estilo json.
*/
char * cmdStackJsonEncode(stack * cmdStack);

/*
	Abre o navegador padrão defininido no Sistema operacional
*/
void openDefaultBrowser(char *fileName);


int main (){
	sysClear();

	puts(MAIN_SCREEN_TITLE);
	puts(HELP_HINT);
	puts(HELP_SCREEN_HINT_TAB);

	cmdList * cmdsList = initCommands(commands, (int) sizeof(commands)/sizeof(*commands));
	stack * cmdStack = newStack(inputCmd*, 0);
	inputCmd * input = NULL;

	//Laço principal do programa
	while(true){
		input = getConsoleInput(cmdsList, 50);
		putchar('\n');

		if(input == NULL){
			fputs(CONSOLE_OUTPUT_SYMBLE, stdout);
			puts(COMMAND_NOT_FOUND);
			continue;
		}

		if(input->statement->command->cmdId == 7){
			fputs(CONSOLE_OUTPUT_SYMBLE, stdout);
			puts(EXIT_CONSOLE);
			break;
		}

		if(input->statement->command->cmdId == -1){
			continue;
		}

		if(input->statement->command->cmdId == 6){
			fputs(CONSOLE_OUTPUT_SYMBLE, stdout);

			if(stackIsEmpy(cmdStack)){
				puts(COMMAND_STACK_EMPTY);
				continue;
			}
			
			printf(UNDO_COMMAND, getTop(cmdStack, inputCmd*)->statement->command->statement);
			popStack(cmdStack);
			continue;
		}

		if(input->statement->command->cmdId == 8){
			printHelpScreen(cmdsList);
		}

		if(input->statement->command->cmdId == 9){
			sysClear();
			continue;
		}

		if(input->statement->command->paramType == 'i'){
			if(input->paramValue == NULL){
				fputs(CONSOLE_OUTPUT_SYMBLE, stdout);
				puts(INVALID_PARAM);
				continue;
			}

			if(atoi(input->paramValue) <= 0){
				fputs(CONSOLE_OUTPUT_SYMBLE, stdout);
				puts(INVALID_PARAM);
				continue;
			}
		}

		if(input->statement->command->stackable){
			pushStack(cmdStack, input);
		}
	}

	char * fileName;
	readString(fileName, REQUIRE_FILENAME);
	sprintf(fileName, "%s.html", fileName);
	
	exportHtmlToFile(fileName, getHtmlTpl(getHeadTpl(), getBodyTpl(getScript(cmdStackJsonEncode(cmdStack)))));
	printf(FILE_SAVED, fileName);

	printf(OPEN_BROWSER_QUESTION, fileName);
	if(getBool("s", "n", "Sim ou Nao", 0)){
		openDefaultBrowser(fileName);
	}

	sysClear();
	return 0;
}


/* Implementações */

void printHelpScreen(cmdList * cmds){

	printf(HELP_SCREEN_TITLE);

	cmdNode * c = getFirstCmdNode(cmds);
	puts(HELP_SCREEN_HINT_CMDS);
	while(c != NULL){
		if(c->command->cmdId >= 0){
			printf("\t* %s:\n\t    - Parametro: ", c->command->statement);

			if(c->command->paramType == 'i'){
				printf(HELP_SCREEN_PARAM_TYPE_I);
			}
			else{
				printf(HELP_SCREEN_PARAM_TYPE_N);
			}
			printf("\t    - Descrição: %s\n\n", c->command->desc);
		}
		c = getNextCmdNode(c);
	}
	puts(HELP_SCREEN_HINT_TAB);
}

char * cmdStackJsonEncode(stack * cmdStack){
	int size = 0;

	stack * temp = newStack(inputCmd*, 0);

	while(!stackIsEmpy(cmdStack)){
		size += 23 + (int) strlen(getTop(cmdStack, inputCmd*)->statement->command->statement);
		
		if(getTop(cmdStack, inputCmd*)->statement->command->paramType == 'i'){
			size += (int) strlen(getTop(cmdStack, inputCmd*)->paramValue);
		}
		else{
			size += 4;
		}

		pushStack(temp, getTop(cmdStack, inputCmd*));

		popStack(cmdStack);
	}

	char * json = (char *) malloc(size + 1);

	while(!stackIsEmpy(temp)){
		if(getTop(temp, inputCmd*)->statement->command->paramType == 'i'){
			sprintf(json, "%s{statement:'%s',param:%s},\n", json, getTop(temp, inputCmd*)->statement->command->statement, getTop(temp, inputCmd*)->paramValue);
		}
		else{
			sprintf(json, "%s{statement:'%s',param:null},\n", json, getTop(temp, inputCmd*)->statement->command->statement);
		}

		popStack(temp);
	}

	return json;
}

void openDefaultBrowser(char *fileName){
	char * invocation = NULL;

	#if defined __APPLE__
	//Abre o navegador no mac
	invocation = (char *) malloc(strlen(OSX_INVOCATION) + strlen(fileName) + 1);
	sprintf(invocation, OSX_INVOCATION, fileName);
	#else
	//Abre o navegador em distriuições derivadas do debian
	invocation = malloc(strlen(LINUX_INVOCATION) + strlen(fileName) + 1);
	sprintf(invocation, LINUX_INVOCATION, fileName);
	#endif

	system(invocation);
	free(invocation);
}
