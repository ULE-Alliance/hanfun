// =============================================================================
/*!
 * \file       /Lana/apps/hanfun_agent.cpp
 *
 * This file contains an example for a Lantiq's HAN-FUN Agent.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <unistd.h>

#include <forward_list>
#include <algorithm>

#include "hanfun.h"

#include "base.h"

#include "application.h"

// =============================================================================
// Defines
// =============================================================================

#define HF_MAX_REG      5
#define HF_IPUI_SIZE    5

#ifndef HF_BASE_APP_PREFIX
#define HF_BASE_APP_PREFIX    "."
#endif

// =============================================================================
// Global Variables
// =============================================================================

const std::string Base::path_prefix = HF_BASE_APP_PREFIX;

static Base base;

// =============================================================================
// Helper Functions
// =============================================================================

// =============================================================================
// print
// =============================================================================
/*!
 * Print the HAN-FUN registration entries.
 *
 * @param devices    a reference to the vector containing the registration entries.
 */
// =============================================================================
static void print (const vector<HF::Core::DeviceManagement::Device*> &devices)
{
   std::cout << "HAN-FUN" << std::endl <<
         "\tRegistered Devices (" << (int) devices.size() << "):" << std::endl;

   std::for_each(devices.begin(), devices.end(), [](const HF::Core::DeviceManagement::Device* device)
   {
      std::cout << setw(9) << (base.link(device->address) != nullptr ? "+ " : "- ");
      std::cout << setw(5) << device->address <<" | " << std::endl;

      for(uint16_t i = 0; i < HF_IPUI_SIZE; i++)
      {
         std::cout << hex << setw(2) << setfill('0') << (int) ((HF::UID::IPUI *)device->uid)->value[i];
      }
      std::cout << endl;
   });
}

// =============================================================================
// print
// =============================================================================
/*!
 * Print the application binding entries.
 *
 * @param entries    reference to the list containing the binding entries.
 */
// =============================================================================
static void print (const std::forward_list<BindEntry> &entries)
{
   std::cout << "HAN-FUN   Binds (" << std::distance(entries.begin(),entries.end()) << "):" << endl;
   std::for_each(entries.begin(), entries.end(), [](const BindEntry &entry) {
      std::cout << "       - " << entry.source << " -> " << entry.destination << std::endl;
   });
}

// =============================================================================
// HandleCommand
// =============================================================================
/*!
 * Handle an application command.
 *
 * @param Key     command key.
 * @param arg1    first argument to the command.
 * @param arg2    second argument to the command.
 */
// =============================================================================
static void HandleCommand (uint8_t Key, uint16_t arg1, uint16_t arg2)
{
   switch (Key)
   {
      case '?':
      {
         std::cout << std::endl;

         std::cout << "================================================" << std::endl;
         std::cout << "HAN-FUN Application Test" << std::endl << std::endl;
         std::cout << "l        : list registrations. " << std::endl;
         std::cout << "l 0      : list binds. " << std::endl;
         std::cout << "r 1 x    : register device x. " << std::endl;
         std::cout << "r 0      : exit registration mode. " << std::endl;
         std::cout << "b x y    : associate device x with device y. (bind) " << std::endl;
         std::cout << "u x y    : unbind device x with y. " << std::endl;
         std::cout << "d x      : deregister device x. " << std::endl;
         std::cout << "h        : help " << std::endl;
         std::cout << "?        : Print This Menu " << std::endl << std::endl;
         std::cout << "Select an Option (Q to exit):" << std::endl << std::endl;

         break;
      }

      case 'l': //!< list registrations.
      {
         std::cout << "=============================================" << std::endl;

         if (arg1 == (uint16_t) HF_MAX_REG && arg2 == (uint16_t) HF_MAX_REG)
         {
            print(base.unit0.management()->entries());
         }
         else if (arg1 == 0) //!< list binds
         {
            print(base.binds());
         }

         std::cout << "=============================================" << std::endl;

         break;
      }

      case 'r': //!< Registration mode
      {
         if (arg1 == 0) //!< Disable Registration
         {
         }
         else if (arg1 == 1) //!< Enable Registration
         {
            if (base.unit0.management()->available(arg2) && arg2 != 0 && arg2 != (uint16_t) HF_MAX_REG)
            {
#ifdef HF_ULE_SUPPORT
               if (HF::ULE::Registration(true))
               {
                  base.unit0.management()->next_address(arg2);
                  std::cout << "[HANFUN] Enable Registration mode: SUCCESS" << std::endl;
                  break;
               }
               else
               {
                  std::cout << "[HANFUN] Enable Registration mode: FAIL" << std::endl;
               }
#endif
               std::cout << "[HANFUN] Next Registration will have address : " << (int) arg2 << std::endl;
            }
            else
            {
               std::cout << "[HANFUN] Registration impossible: address " << (int) arg2
                     << "  not available."<< std::endl;
               break;
            }
         }

         std::cout << "r 0      : Disable Registration Mode." << std::endl;
         std::cout << "r 1 x    : Enable Registration Mode (Register Device x)." << std::endl;

         break;
      }

      case 'b': //!< Associate device x with device y. (bind)
      {
         uint8_t err = base.bind(arg1, arg2);

         switch (err)
         {
            case 0:
               std::cout << "[HANFUN] Bind created !" << std::endl;
               break;
            case 1:
               std::cout << "[HANFUN] Bind already created !" << std::endl;
               break;
            case 2:
            case 3:
               std::cout << "[HANFUN] Bind impossible: " << std::endl;
               if ( (err & 0x01) != 0)
               {
                  std::cout << "\t- Second device does not exist !" << std::endl;
               }
               else
               {
                  std::cout << "\t- Fist device does not exist !" << std::endl;
               }
               break;
            default:
            break;
         }

         break;
      }

      case 'u': //!< Unbind device x with y.
      {
         if (base.has_bind(arg1, arg2))
         {
            base.unbind(arg1, arg2);
            cout << "Bind: " << arg1 << " - " << arg2 << " removed." << endl;
         }
         else
         {
            cout << "Bind does not exist!" << endl;
         }

         break;
      }

      case 'd': // de-register device x.
      {
         if (arg1 == (uint16_t) HF_MAX_REG || arg1 == 0)
         {
            std::cout << "d x      : deregister device x. " << std::endl;
            return;
         }

#ifdef HF_ULE_SUPPORT
         // DECT de-registration
         HF::ULE::Deregister(arg1);
#endif

         /* HAN-FUN de-registration */
         std::cout << "[HANFUN] Device " << (int) arg1 << " de-registration: ";
         if (base.unit0.management()->deregister(arg1))
         {
            cout << "SUCCESS !" ;
            base.unbind(arg1);
         }
         else
         {
            cout << "FAIL !" ;
         }
         std::cout << std::endl;

         break;
      }

      case 'h': // help
      {
         std::cout << "h        : help " << std::endl;
         HandleCommand('?', 0, 0);

         break;
      }

      default:
      {
         std::cout << std::endl << "|| Unknown command: " << hex << (int) Key << "||";
         HandleCommand('?', 0, 0);
         break;
      }
   };
}

// =============================================================================
// MAIN
// =============================================================================

int HF::Application::Base (int argc, char * argv[], HF::Transport::Layer &transport)
{
   UNUSED(argc);
   UNUSED(argv);

   transport.initialize();
   transport.add(&base);

   // User menu to invoke commands from 452 console
   uint8_t Key = '?'; // Start with print of help text
   uint16_t arg1 = (uint16_t) HF_MAX_REG;
   uint16_t arg2 = (uint16_t) HF_MAX_REG;

   HandleCommand('?', 0, 0);
   while (Key != 'Q' /* Use Q key to quit*/)
   {
      std::string inputLine;
      std::getline(std::cin, inputLine);
      std::istringstream stream(inputLine);

      Key = '!';
      arg1 = (uint16_t) HF_MAX_REG;
      arg2 = (uint16_t) HF_MAX_REG;

      stream >> Key >> arg1 >> arg2;

      if (Key != '!' && Key != 'Q')
      {
         HandleCommand(Key, arg1, arg2);
      }
      cout << "> ";
   }

   return 0;
}
