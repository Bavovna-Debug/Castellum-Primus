// System definition files.
//
#include <libconfig.h++>
#include <cstring>
#include <stdexcept>
#include <string>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"

using namespace libconfig;

void
Primus::Configuration::load()
{
    Config &config = this->open();

    try
    {
        Setting &rootSetting = config.getRoot();

        Setting &primusSetting = rootSetting["Primus"];

        {
            Setting &databaseSetting = primusSetting["Database"];

            try
            {
                const std::string hostName      = databaseSetting["HostName"];
                const unsigned int portNumber   = databaseSetting["PortNumber"];
                const std::string databaseName  = databaseSetting["DatabaseName"];
                const std::string role          = databaseSetting["Role"];
                const std::string password      = databaseSetting["Password"];

                if ((portNumber == 0) ||
                    (hostName.empty() == true))
                {
                    ReportSoftAlert("[Workspace] Not specified mandatory options in \"Database\" section");

                    throw std::exception();
                }

                this->database.hostName     = new std::string(hostName);
                this->database.portNumber   = portNumber;
                this->database.databaseName = new std::string(databaseName);
                this->database.role         = new std::string(role);
                this->database.password     = new std::string(password);
            }
            catch (SettingNotFoundException &exception)
            {
                ReportSoftAlert("[Workspace] Missing mandatory options in \"Database\" section");

                throw exception;
            }
        }

        this->httpPortNumber             = primusSetting["HTTP_PortNumberIPv4"];
        this->servusPortNumberIPv4       = primusSetting["Servus_PortNumberIPv4"];
        this->servusPortNumberIPv6       = primusSetting["Servus_PortNumberIPv6"];
        this->anticipatorPortNumberIPv4  = primusSetting["Anticipator_PortNumberIPv4"];
        this->anticipatorPortNumberIPv6  = primusSetting["Anticipator_PortNumberIPv6"];

        {
            Setting &networkSetting = primusSetting["Network"];

            this->network.servus.waitForFirstDatagram       = networkSetting["ServusWaitForFirstDatagram"];
            this->network.servus.waitForDatagramCompletion  = networkSetting["ServusWaitForDatagramCompletion"];
            this->network.servus.intervalBetweenNeutrinos   = networkSetting["ServusIntervalBetweenNeutrinos"];
            this->network.servus.finalWaitForNeutrino       = networkSetting["ServusFinalWaitForNeutrino"];

            this->network.phoenix.waitForFirstDatagram      = networkSetting["PhoenixWaitForFirstDatagram"];
            this->network.phoenix.waitForDatagramCompletion = networkSetting["PhoenixWaitForDatagramCompletion"];
            this->network.phoenix.keepAlive                 = networkSetting["PhoenixKeepAlive"];
        }

        {
            Setting &apnsSetting = primusSetting["APNS"];

            try
            {
                const std::string certificate = apnsSetting["Certificate"];

                this->apns.sandbox = apnsSetting["Sandbox"];
                this->apns.certificate = new std::string(certificate);
            }
            catch (SettingNotFoundException &exception)
            {
                ReportSoftAlert("[Workspace] Missing mandatory options in \"APNS\" section");

                throw exception;
            }

            this->apns.delayAfterWakeup      = apnsSetting["DelayAfterWakeup"];
            this->apns.delayBetweenFrames    = apnsSetting["DelayBetweenFrames"];
            this->apns.delayAfterCompletion  = apnsSetting["DelayAfterCompletion"];
            this->apns.pauseBeforeReconnect  = apnsSetting["PauseBeforeReconnect"];
        }
    }
    catch (SettingNotFoundException &exception)
    {
        ReportError("[Workspace] Using default for \"%s\"",
                exception.getPath());
    }
    catch (SettingTypeException &exception)
    {
        ReportError("[Workspace] Exception on configuration: %s (%s)",
                exception.what(),
                exception.getPath());
    }
    catch (std::exception &exception)
    {
        ReportError("[Workspace] Exception on configuration: %s",
                exception.what());
    }

    this->close(config);
}
