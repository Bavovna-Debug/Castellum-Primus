// System definition files.
//
#include <libconfig.h++>
#include <algorithm>
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

        // Database block.
        //
        {
            Setting &databaseSetting = rootSetting["Database"];

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

                this->database.hostName     = hostName;
                this->database.portNumber   = portNumber;
                this->database.databaseName = databaseName;
                this->database.role         = role;
                this->database.password     = password;
            }
            catch (SettingNotFoundException &exception)
            {
                ReportSoftAlert("[Workspace] Missing mandatory options in \"Database\" section");

                throw exception;
            }
        }

        // HTTP block.
        //
        {
            Setting &httpSetting = rootSetting["HTTP"];

            unsigned int portNumber         = httpSetting["PortNumberIPv4"];
            std::string passwordMD5         = httpSetting["PasswordMD5"];
            unsigned int keepAliveSession   = httpSetting["KeepAliveSession"];

            std::transform(passwordMD5.begin(), passwordMD5.end(),passwordMD5.begin(), ::toupper);

            this->http.portNumber = portNumber;
            this->http.passwordMD5 = passwordMD5;
            this->http.keepAliveSession = keepAliveSession;
        }

        // Servus block.
        //
        {
            Setting &servusSetting = rootSetting["Servus"];

            unsigned int portNumberIPv4 = servusSetting["PortNumberIPv4"];
            unsigned int portNumberIPv6 = servusSetting["PortNumberIPv6"];

            this->servus.portNumberIPv4 = portNumberIPv4;
            this->servus.portNumberIPv6 = portNumberIPv6;

            this->servus.waitForFirstDatagram       = servusSetting["WaitForFirstDatagram"];
            this->servus.waitForDatagramCompletion  = servusSetting["WaitForDatagramCompletion"];
            this->servus.intervalBetweenNeutrinos   = servusSetting["IntervalBetweenNeutrinos"];
            this->servus.finalWaitForNeutrino       = servusSetting["FinalWaitForNeutrino"];
        }

        // Anticipator block.
        //
        {
            Setting &anticipatorSetting = rootSetting["Anticipator"];

            try
            {
                const std::string interfaceAddress = anticipatorSetting["InterfaceAddress"];

                this->phoenix.interfaceAddress = interfaceAddress;
            }
            catch (SettingNotFoundException &exception)
            {
                ReportSoftAlert("[Workspace] Missing mandatory option in \"Anticipator\" section");

                throw exception;
            }

            unsigned int portNumberIPv4 = anticipatorSetting["PortNumberIPv4"];
            unsigned int portNumberIPv6 = anticipatorSetting["PortNumberIPv6"];

            this->phoenix.portNumberIPv4 = portNumberIPv4;
            this->phoenix.portNumberIPv6 = portNumberIPv6;

            this->phoenix.waitForFirstDatagram      = anticipatorSetting["WaitForFirstDatagram"];
            this->phoenix.waitForDatagramCompletion = anticipatorSetting["WaitForDatagramCompletion"];
            this->phoenix.delayResponseForActivate  = anticipatorSetting["DelayResponseForActivate"];
            this->phoenix.delayResponseForLogin     = anticipatorSetting["DelayResponseForLogin"];
            this->phoenix.delayResponseForRejected  = anticipatorSetting["DelayResponseForRejected"];
            this->phoenix.keepAlive                 = anticipatorSetting["KeepAlive"];
        }

        // APNS block.
        //
        {
            Setting &apnsSetting = rootSetting["APNS"];

            try
            {
                const std::string certificate = apnsSetting["Certificate"];

                this->apns.sandbox = apnsSetting["Sandbox"];
                this->apns.certificate = certificate;
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
