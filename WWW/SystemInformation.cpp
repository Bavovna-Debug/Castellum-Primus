// Common definition files.
//
#include "HTTP/Connection.hpp"
#include "HTTP/HTML.hpp"
#include "HTTP/HTTP.hpp"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Kernel.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'System Information' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void
WWW::Site::pageSystemInformation(HTTP::Connection& connection, HTML::Instance& instance)
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    HTML::Division division(instance, HTML::Nothing, "workspace");

    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("System information");
        } // HTML.HeadingText

        {
            HTML::Table table(instance);

            {
                HTML::Caption caption(instance);

                caption.plain("Software");
            }

            {
                HTML::TableBody tableBody(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Version:");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%s", Primus::SoftwareVersion.c_str());
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Läuft seit:");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        Workspace::Kernel& kernel = Workspace::Kernel::SharedInstance();
                        tableDataCell.plain(kernel.timestampOfStart->YYYYMMDDHHMM());
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Konfigurationsdatei:");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%s", configuration.configurationFilePath.c_str());
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("APNS SSL-Zertifikat:");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%s (%s)",
                                configuration.apns.certificate->c_str(),
                                (configuration.apns.sandbox == false) ? "Deployment" : "Development");
                    }
                }
            }
        }

        {
            HTML::Table table(instance);

            {
                HTML::Caption caption(instance);

                caption.plain("Netzwerkeinstellungen");
            }

            {
                HTML::TableBody tableBody(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Webseite TCP Portnummer (IPv4):");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%u", configuration.http.portNumber);
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Servus TCP Portnummer (IPv4):");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%u", configuration.servus.portNumberIPv4);
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Servus TCP Portnummer (IPv6):");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%u", configuration.servus.portNumberIPv6);
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Anticipator TCP Portnummer (IPv4):");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%u", configuration.phoenix.portNumberIPv4);
                    }
                }

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableHeadCell tableHeadCell(instance);

                        tableHeadCell.plain("Anticipator TCP Portnummer (IPv6):");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("%u", configuration.phoenix.portNumberIPv6);
                    }
                }
            }
        }
    }
}
#pragma GCC diagnostic pop
