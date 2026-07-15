#include "invoke_cli.h"

int main(int argc, char** argv)
{
	return tournament_builder::tournament_builder_internal::invoke_cli(argc, argv);
}