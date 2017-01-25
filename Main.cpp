//
// Purpose: A command-line tool displays all modules loaded by a particular process
//

// windows API headers
#include <Windows.h>
#include <Psapi.h>

// standard headers
#include <iostream>

// boost
#include <boost/program_options.hpp>

// the boost command line options namespace
namespace options = boost::program_options;

using namespace std;

// application entry point
int main (int argc, char* argv[])
{
   // optimism
   auto result = EXIT_SUCCESS;

   // declare the supported command-line options.
   options::options_description desc ("Usage");
   desc.add_options ()
      ("help,h", "Display usage information.")
      ("process-id,p", options::value<DWORD> (), "The Process ID for which to enumerate loaded modules.");

   options::variables_map variables_map;
   options::store (options::parse_command_line (argc, argv, desc), variables_map);
   options::notify (variables_map);

   // if help is requested, show help and exit
   if (variables_map.count ("help"))
   {
      cout << desc << endl;
      // early return
      return EXIT_FAILURE;
   }

   try
   {
      if (!variables_map.count ("process-id"))
      {
         // if no ID is specified, throw an exception
         throw exception ("No process ID specified.");
      }

      auto process_id = variables_map["process-id"].as<DWORD> ();

      HANDLE process_handle = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
      if (NULL == process_handle)
      {
         throw exception ("Could not retrieve process handle.");
      }

      cout << "Modules loaded by Process ID: " << process_id << endl;

      HMODULE module_handles[1024];
      DWORD bytes_needed;

      // note that because this project is built for x86, it cannot enumerate modules for 64-bit processes
      if (EnumProcessModules (process_handle, module_handles, sizeof (module_handles), &bytes_needed))
      {
         for (unsigned int index = 0; index < (bytes_needed / sizeof (HMODULE)); index++)
         {
            char module_name[MAX_PATH];
            // Get the full path to the module's file.
            if (GetModuleFileNameExA (process_handle, module_handles[index], module_name, sizeof (module_name)))
            {
               // Print the module name and handle value.
               cout << '\t' << module_name << " (0x" << hex << module_handles[index] << dec << ")" << endl;
            }
         }
      }
      else
      {
         string error_message = "EnumProcessModules failed with error code: " + to_string (GetLastError ());
         throw exception (error_message.c_str ());
      }

      CloseHandle (process_handle);
   }
   catch (exception& exception)
   {
      // on any exception, display the exception message and the application's usage
      cerr << "Error: " << exception.what () << endl;
      cerr << endl;
      cerr << desc << endl;
      result = EXIT_FAILURE;
   }

   return result;
}