/*
 * rss.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/config.h>
#include <vdr/plugin.h>

#include "i18n.h"
#include "menu.h"
#include "config.h"
#include "common.h"

#if defined(VDRVERSNUM) && VDRVERSNUM < 10337
#error "You don't exist! Go away! Upgrade yourself!"
#endif

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = "RSS Reader for OSD";
static const char *MAINMENUENTRY  = "RSS Reader";

class cPluginRssReader : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRssReader(void);
  virtual ~cPluginRssReader();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return (RssConfig.hidemenu ? NULL : tr(MAINMENUENTRY)); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

class cPluginRssReaderSetup : public cMenuSetupPage
{
private:
  cRssReaderConfig data;
protected:
  virtual void Store(void);
public:
  cPluginRssReaderSetup(void);
};

cPluginRssReader::cPluginRssReader(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRssReader::~cPluginRssReader()
{
  // Clean up after yourself!
}

const char *cPluginRssReader::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "  -f <RSSTEMPFILE>, --file=<RSSTEMPFILE>  Define a temporary file for RSS Reader (default: " RSSTEMPFILE ")\n";
}

bool cPluginRssReader::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "file",      required_argument, NULL, 'f' },
       { NULL }
     };

  int c;
  while ((c = getopt_long(argc, argv, "f:", long_options, NULL)) != -1) {
        switch (c) {
          case 'f': RssConfig.tempfile = optarg;
                    break;
          default:  return false;
          }
        }
  return true;
}

bool cPluginRssReader::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  return true;
}

bool cPluginRssReader::Start(void)
{
  // Start any background activities the plugin shall perform.
  if (!RssItems.Load(AddDirectory(ConfigDirectory(), "rssreader.conf")))
     error("configuration file 'rssreader.conf' not found!");
  return true;
}

void cPluginRssReader::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRssReader::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRssReader::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cRssStreamsMenu();
}

cMenuSetupPage *cPluginRssReader::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPluginRssReaderSetup;
}

bool cPluginRssReader::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "HideMenu")) RssConfig.hidemenu = atoi(Value);
  else if (!strcasecmp(Name, "HideElem")) RssConfig.hideelem = atoi(Value);
  else return false;

  return true;
}

bool cPluginRssReader::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRssReader::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRssReader::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

cPluginRssReaderSetup::cPluginRssReaderSetup(void)
{
  data = RssConfig;
  
  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &data.hidemenu, tr("no"), tr("yes")));
  Add(new cMenuEditBoolItem(tr("Hide non-existent elements"), &data.hideelem, tr("no"), tr("yes")));
}

void cPluginRssReaderSetup::Store(void)
{
  RssConfig = data;
  SetupStore("HideMenu", RssConfig.hidemenu);
  SetupStore("HideElem", RssConfig.hideelem);
}

VDRPLUGINCREATOR(cPluginRssReader); // Don't touch this!
