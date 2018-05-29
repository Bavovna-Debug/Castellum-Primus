// System definition files.
//
#include <cstdbool>
#include <string>

// Common definition files.
//
#include "HTTP/Connection.hpp"
#include "HTTP/HTML.hpp"
#include "HTTP/HTTP.hpp"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Phoenix' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pagePhoenix(HTTP::Connection& connection, HTML::Instance& instance)
{
    // Process forms.
    //
    {
        if (connection.argumentPairExists(WWW::Action, WWW::ActionPhoenixEdit) == true)
        {
            this->pagePhoenixEditForm(connection, instance);

            return;
        }

        if (this->formSubmitted(connection) == true)
        {
            if (connection.argumentPairExists(WWW::Action, WWW::ActionPhoenixSave) == true)
            {
                try
                {
                    const unsigned long phoenixId = connection[WWW::PhoenixId];

                    try
                    {
                        const std::string phoenixDescription = connection[WWW::PhoenixDescription];
                        if (phoenixDescription.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        Database::Phoenix& phoenix = Database::Phoenixes::PhoenixById(phoenixId);

                        phoenix.setDescription(phoenixDescription);

                        delete &phoenix;
                    }
                    catch (HTTP::ArgumentDoesNotExist&)
                    {
                        instance.errorMessage("Beschreibung fehlt!");

                        this->pagePhoenixEditForm(connection, instance);

                        return;
                    }
                }
                catch (HTTP::ArgumentDoesNotExist&)
                {
                    instance.alertMessage("Fehler in Browser!");

                    return;
                }
            }
        }
    }

    HTML::Division division(instance, HTML::Nothing, "workspace");

    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Phoenix");
        } // HTML.HeadingText

        {
            HTML::Table table(instance);

            {
                HTML::TableHeader tableHeader(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Aktiviert");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Name");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Model");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("App");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Beschreibung");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }
                }
            }

            {
                HTML::TableBody tableBody(instance);

                unsigned long numberOfPhoenixes = Database::Phoenixes::TotalNumber();
                for (unsigned int phoenixIndex = 0;
                     phoenixIndex < numberOfPhoenixes;
                     phoenixIndex++)
                {
                    Database::Phoenix& phoenix = Database::Phoenixes::PhoenixByIndex(phoenixIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(phoenix.timestamp->YYYYMMDD());
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(phoenix.deviceName);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(phoenix.deviceModel);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(phoenix.softwareVersion);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(phoenix.description);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionPhoenixEdit.c_str(),
                                    WWW::PhoenixId.c_str(),
                                    std::to_string(phoenix.phoenixId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Bearbeiten.");

                            url.image("img/edit.png", "Bearbeiten.");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    delete &phoenix;
                }
            }
        }
    } // HTML.Division
}

/**
 * @brief   Generate HTML page for the 'Phoenix' edit form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pagePhoenixEditForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long phoenixId;

    try
    {
        phoenixId = connection[WWW::PhoenixId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        phoenixId = 0;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "observatorium",
            "observatorium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionPhoenixSave);

    form.hidden(WWW::PhoenixId, phoenixId);

    std::string phoenixDescription;

    if (phoenixId != 0)
    {
        Database::Phoenix& phoenix = Database::Phoenixes::PhoenixById(phoenixId);

        phoenixDescription = phoenix.description;

        delete &phoenix;
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            if (phoenixId == 0)
            {
                instance.alertMessage("Fehler in Browser!");

                return;
            }

            headingText.plain("Phoenix <b>%s</b> bearbeiten", phoenixDescription.c_str());
        } // HTML.HeadingText

        {
            HTML::DefinitionList definitionList(instance);

            {
                HTML::DefinitionTerm definitionTerm(instance);

                {
                    HTML::Label label(instance);

                    label.plain("Beschreibung");
                }
            }

            {
                HTML::DefinitionDescription definitionDescription(instance);

                form.textField("description", "inputbox",
                        WWW::PhoenixDescription,
                        phoenixDescription.c_str(),
                        100, 40);
            }
        }
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "south");

        {
            HTML::Button submitButton(instance,
                    HTML::Nothing,
                    HTML::Nothing,
                    WWW::Button,
                    WWW::ButtonSubmit);

            submitButton.plain("Speichern");
        }

        {
            HTML::Button cancelButton(instance,
                    HTML::Nothing,
                    HTML::Nothing,
                    WWW::Button,
                    WWW::ButtonCancel);

            cancelButton.plain("Abbrechen");
        }
    }
}
