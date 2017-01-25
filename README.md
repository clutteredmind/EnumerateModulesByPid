# Enumerate Modules by PID	

This command-line program allows you to see all the DLLs currently in use by any 32-bit process.

Use -h or --help to see usage information.

Use -p or --process-id to specify the process ID to enumerate. Alternatively, you can input the process ID by itself and the application will figure it out.

Note that you'll need to have the boost libraries (specifically the program_options lib) available in order to build the project.