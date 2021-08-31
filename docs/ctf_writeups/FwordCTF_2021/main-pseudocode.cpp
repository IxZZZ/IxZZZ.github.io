int __cdecl main(int argc, const char **argv, const char **envp)
{
  int result; // eax

  sub_401E80();
  if ( getenv("joezidsecret") )
    result = handle_1();
  else
    result = handle_2(*argv);
  return result;
}
