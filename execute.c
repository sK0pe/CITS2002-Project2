#include "mysh.h"
#include <errno.h>

/*
	CITS2002 Project 2 2015
	Name:			Pradyumn Vij
	Student number:	21469477
	Date:			26/10/2015
*/

/*
 *  execute_cmdtree
 *  
 *  input: CMDTREE pointer
 *  return: integer
 *
 *  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
 *  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
 *  
 */
int execute_cmdtree(CMDTREE *t){
	int exitstatus;
	//	If CMDTREE is null return failure.
	if(t == NULL){
		return(EXIT_FAILURE);
	}else{
		exitstatus = EXIT_SUCCESS;
	}
	//  Check type of Command Branch
	switch(t->type){
		case N_COMMAND:{
			//  Check for builtin tasks
			//  Exit task
			if(strcmp(t->argv[0], "exit") == 0){
				mysh_exit(t);
			}
			//  Timing task
			if(strcmp(t->argv[0], "time") == 0){
				//  Pass memory address for 2nd argument
				exitstatus = mysh_time(t);
				break;
			}
			//  Change Directory
			if(strcmp(t->argv[0], "cd") == 0){
				//  Pass memory address for 2nd argument
				exitstatus = mysh_cd(&t->argv[1]);
				break;
			}
			//  Change Directory
			if(strcmp(t->argv[0], "set") == 0){
				//  Pass arg count-1 and memory address for 2nd argument
				exitstatus = mysh_set(--t->argc, &t->argv[1]);
				break;
			}
			//  Fork and execute external command(s)
			exitstatus = launch_command(t);
			break;
		}
		case N_SEMICOLON:{
			//  Execute left branch
			execute_cmdtree(t->left);
			//  Right branch represents exit status.
			exitstatus = execute_cmdtree(t->right);
			break;
		}
		case N_AND:{
			//  Execute left branch
			exitstatus = execute_cmdtree(t->left);
			//  If left branch succeeds, execute right branch
			//  Exit status is generated by th last executed command
			if(exitstatus == EXIT_SUCCESS){
				exitstatus = execute_cmdtree(t->right);
			}
			break;
		}
		case N_OR:{
			//  Execute right branch
			exitstatus = execute_cmdtree(t->left);
			//  If left branch fails, execute right branch
			//  Exit status is generated by the last executed command
			if(exitstatus == EXIT_FAILURE){
				exitstatus = execute_cmdtree(t->right);
			}
			break;
		}
		case N_BACKGROUND:{
			//  Launch left branch in background
			exitstatus = launch_background(t->left);
			//  Launch right branch
			//  Do not wait for it, will continue to run till mysh
			//  exits.  Then inherited by init.
			execute_cmdtree(t->right);
			break;
		}
		case N_SUBSHELL:{
			//  Fork shell and execute remainining cmdtree in child shell;
			//launch_subshell(t, &exitstatus);
			exitstatus = launch_command(t);
			break;
		}
		case N_PIPE:{
			//  Use STDOUT of left branch as STDIN for right branch
			exitstatus = launch_pipe(t);
			break;
		}
		default :
			fprintf(stderr,"%s: invalid NODETYPE in print_cmdtree0()\n",argv0);
			exitstatus = EXIT_FAILURE;
			break;
	}
	return exitstatus;
}
