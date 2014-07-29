// =============================================================================
/*!
 * \file       apps/common.h
 *
 * This file contains the definitions for the common funtionality in the HAN-FUN
 * example applications.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_APP_COMMON_H
#define HF_APP_COMMON_H

#include <map>

#include "hanfun.h"

// =============================================================================
// Defines
// =============================================================================

// =============================================================================
// Command API
// =============================================================================

struct ICommand
{
   protected:

   static std::map <std::string, ICommand *> registry;

   public:

   virtual const std::string &key () const = 0;

   virtual const std::string &usage () const                       = 0;

   virtual void              run (std::vector <std::string> &args) = 0;

   static void               add (ICommand *command);

   static void               remove (ICommand *command);

   static void               run (std::string &cmd, std::vector <std::string> &args);

   static std::ostream       &help (std::ostream &stream);
};

class Command:public ICommand
{
   protected:

   const std::string _key;
   const std::string _usage;

   public:

   Command(const char *__key, const char *__usage):
      _key (__key), _usage (__usage)
   {}

   const std::string &key () const
   {
      return _key;
   }

   const std::string &usage () const
   {
      return _usage;
   }
};

#define COMMAND(_name, _key, _help)               \
   struct Command_## _name:public Command         \
   {                                              \
      Command_##_name ():Command (_key, _help) {} \
                                                  \
      void run (std::vector <std::string> &args); \
   };                                             \
   Command_##_name command##_name;                \
   void Command_##_name::run (std::vector <std::string> &args)

#define COMMAND_ADD(_name)             \
   {                                   \
      ICommand::add (&command##_name); \
   }

// =============================================================================
// Stream Helpers
// =============================================================================

ostream &operator <<(ostream &stream, HF::Common::ByteArray const &array);

ostream &operator <<(ostream &stream, const HF::UID::UID *uid);

ostream &operator <<(ostream &stream, const HF::Protocol::Packet &packet);

// =============================================================================
// Parser helpers
// =============================================================================

#define STRTOL(X)       strtol (X.c_str (), NULL, 10);
#define STRTOL_HEX(X)   strtol (X.c_str (), NULL, 16);

#endif /* HF_APP_COMMON_H */
