// =============================================================================
/*!
 * \file       apps/common.cpp
 *
 * This file contains the implementation of the common functionality for the
 * HAN-FUN example application.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

#include <cstdint>

#include "common.h"
#include "application.h"

// =============================================================================
// Defines
// =============================================================================

// =============================================================================
// Streams Helpers
// =============================================================================

ostream & operator <<(ostream &stream, HF::Common::ByteArray const &array)
{
   if (stream == std::cout || stream == std::cerr)
   {
      std::ios_base::fmtflags ff;
      ff = stream.flags ();
      char f = stream.fill ('0');

      stream << std::hex << std::setw (2) << std::setfill ('0');
      stream << "(" << array.size () << ") ";

      for (auto byte : array)
      {
         stream << (int) byte << " ";
      }

      stream.setf (ff);
      stream.fill (f);
   }
   else
   {
      for_each (array.begin (), array.end (), [&stream](uint8_t byte) {
                   stream << byte;
                }
               );
   }

   return stream;
}

ostream &operator <<(ostream &stream, const HF::UID::UID *uid)
{
   std::ios_base::fmtflags ff;
   ff = stream.flags ();
   char f = stream.fill ('0');

   if (uid == nullptr)
   {
      stream << "(null)";
   }
   else
   {
      switch (uid->type ())
      {
         case HF::UID::NONE_UID:
         {
            stream << "uid: NONE";
            break;
         }
         case HF::UID::RFPI_UID:
         {
            HF::UID::RFPI *rfpi = (HF::UID::RFPI *) uid;
            stream << "rfpi: ";

            for (uint8_t i = 0; i < sizeof(rfpi->value); i++)
            {
               stream << std::hex << std::setw (2) << std::setfill ('0');
               stream << (int) rfpi->value[i];
            }

            break;
         }

         case HF::UID::IPUI_UID:
         {
            HF::UID::IPUI *ipui = (HF::UID::IPUI *) uid;
            stream << "ipui: ";

            for (uint8_t i = 0; i < sizeof(ipui->value); i++)
            {
               stream << std::hex << std::setw (2) << std::setfill ('0');
               stream << (int) ipui->value[i];
            }

            break;
         }

         case HF::UID::MAC_UID:
         {
            HF::UID::MAC *mac = (HF::UID::MAC *) uid;
            stream << "mac: ";

            for (uint8_t i = 0; i < sizeof(mac->value); i++)
            {
               stream << std::hex << std::setw (2) << std::setfill ('0');
               stream << (int) mac->value[i] << ":";
            }

            break;
         }

         case HF::UID::URI_UID:
         {
            HF::UID::URI *uri = (HF::UID::URI *) uid;
            stream << "uri: " << uri->value;
            break;
         }

         default:
            break;
      }
   }

   stream.setf (ff);
   stream.fill (f);

   return stream;
}

ostream &operator <<(ostream &stream, const HF::Protocol::Packet &packet)
{
   std::ios_base::fmtflags ff;
   ff = stream.flags ();
   char f = stream.fill (0);

   stream << std::hex << std::setw (2);

   stream << "Packet : " << NL;

   stream << "  Source  : "
          << (int) packet.source.mod << " "
          << (int) packet.source.device << " "
          << (int) packet.source.unit << NL;

   stream << "  Dest    : "
          << (int) packet.destination.mod << " "
          << (int) packet.destination.device << " "
          << (int) packet.destination.unit << NL;

   stream << "  Message :" << NL;

   stream << "    Reference: "
          << (int) packet.message.reference << NL;

   stream << "    Type     : "
          << (int) packet.message.type << NL;

   stream << "    Interface: "
          << (int) packet.message.itf.role << " "
          << (int) packet.message.itf.id << " "
          << (int) packet.message.itf.member << NL;

   stream << "    Length   : " << std::showbase
          << (int) packet.message.length << NL;

   stream << std::noshowbase;

   stream.setf (ff);
   stream.fill (f);

   return stream;
}

// =============================================================================
// Commands API
// =============================================================================

//! Command registry.
std::map <std::string, ICommand *> ICommand::registry;

// =============================================================================
// ICommand::add
// =============================================================================
/*!
 * Add a command to the registry.
 *
 * @param [in] command  pointer to the command to add to the registry.
 */
// =============================================================================
void ICommand::add (ICommand *command)
{
   registry.insert (std::pair <std::string, ICommand *>(command->key (), command));
}


// =============================================================================
// ICommand::remove
// =============================================================================
/*!
 * Remove a command form the registry.
 *
 * @param [in] command  pointer to the command to remove from the registry.
 */
// =============================================================================
void ICommand::remove (ICommand *command)
{
   registry.erase (command->key ());
}

// =============================================================================
// ICommand::help
// =============================================================================
/*!
 * Generate the help screen, based on the commands in the registry.
 *
 * @param [in] stream   reference to the stream to print the help screen.
 *
 * @return  reference to the stream.
 */
// =============================================================================
std::ostream &ICommand::help (std::ostream &stream)
{
   struct entry
   {
      std::string cmd;
      std::string help;
   };

   uint16_t size = 0;
   std::vector <entry> entries;

   std::for_each (ICommand::registry.begin (), ICommand::registry.end (),
                  [&entries, &size](std::pair <std::string, ICommand *> e)
                  {
                     // LOG (TRACE) << "E F : " << e.first << NL;
                     // LOG (TRACE) << "E S : " << e.second << NL;

                     const std::string &raw = e.second->usage ();

                     char *temp = new char[raw.size ()];

                     strncpy (temp, raw.c_str (), raw.size ());

                     std::vector <char *> lines;

                     char *p = strtok (temp, "|");

                     while (p)
                     {
                        lines.push_back (p);
                        p = strtok (NULL, "|");
                     }

                     std::for_each (lines.begin (), lines.end (), [&entries, &size](char *line)
                                    {
                                       char *p = strtok (line, ":");
                                       entry e;
                                       e.cmd = std::string (p);
                                       p = strtok (NULL, ":");
                                       e.help = std::string (p);
                                       entries.push_back (e);

                                       if (size < e.cmd.size ())
                                       {
                                          size = e.cmd.size ();
                                       }
                                    }
                                   );

                     delete[] temp;
                  }
                 );

   size++;

   stream << std::endl;

   stream << "================================================" << std::endl;
   stream << "HAN-FUN Example Application" << std::endl << std::endl;

   stream << setfill (' ');
   std::for_each (entries.begin (), entries.end (), [&stream, size](entry &e) {
                     stream << std::left << std::setw (size) << e.cmd << "\t: " << e.help << std::endl;
                  }
                 );

   stream << std::left << std::setw (size) << "h/?" << "\t: " << "this help menu."
          << std::endl << std::endl;
   stream << "Select an Option (Q/q to exit): " << std::endl;

   return stream;
}

// =============================================================================
// ICommand::run
// =============================================================================
/*!
 * Run the command with the given \c key, using the arguments in \c args.
 *
 * @param [in] cmd   the key for the command to be run.
 * @param [in] args  vector containing the arguments to call the command with.
 */
// =============================================================================
void ICommand::run (std::string &key, std::vector <std::string> &args)
{
   auto it = registry.find (key);

   if (it == registry.end ())
   {
      LOG (ERROR) << "Command '" << key << "' not found !" << NL;
      ICommand::help (std::cout);
   }
   else
   {
      it->second->run (args);
   }
}

// =============================================================================
// API
// =============================================================================

// =============================================================================
// HF::Application::Handle
// =============================================================================
/*!
 *
 */
// =============================================================================
bool HF::Application::Handle (std::string command)
{
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (command.empty () || (command.size () == 1 && command[0] == '\n'))
   {
      ICommand::help (std::cout);
      return false;
   }

   std::istringstream buf (command);
   std::istream_iterator <std::string> beg (buf), end;

   std::vector <std::string> tokens (beg, end);

   std::string cmd = *tokens.begin ();

   //   LOG (TRACE) << "CMD : " << cmd << NL;

   std::vector <std::string> args (tokens.begin () + 1, tokens.end ());

   //   for_each(args.begin(), args.end(), [](std::string &arg){
   //      LOG (TRACE) << "  ARG : " << arg << NL;
   //   });

   if (cmd == "q" || cmd == "Q")
   {
      return true;
   }
   else if (cmd == "h" || cmd == "?")
   {
      LOG (APP) << "";
      ICommand::help (std::cout);
   }
   else
   {
      ICommand::run (cmd, args);
   }

   LOG (APP) << "> " << std::flush;

   return false;
}
