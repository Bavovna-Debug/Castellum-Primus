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

        if (connection.argumentPairExists(WWW::Action, WWW::ActionPhoenixRemove) == true)
        {
            this->generatePhoenixRemoveForm(connection, instance);

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
            else if (connection.argumentPairExists(WWW::Action, WWW::ActionPhoenixRemoveConfirmed) == true)
            {
                try
                {
                    unsigned long phoenixId = connection[WWW::PhoenixId];

                    Database::Phoenix& phoenix = Database::Phoenixes::PhoenixById(phoenixId);

                    instance.noticeMessage("Phoenix <b>%s</b> und zugeordnete Push Notifications " \
                            "wurden unwiderruflich aus dem System entfernen.",
                            phoenix.description.c_str());

                    Database::Phoenixes::RemovePhoenixById(phoenixId);

                    delete &phoenix;
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

    this->pagePhoenixInfo(connection, instance);

    this->pagePhoenixList(connection, instance);
}

/**
 * @brief   Show Phoenix info panel.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pagePhoenixInfo(HTTP::Connection& connection, HTML::Instance& instance)
{ }

/**
 * @brief   Show list of Phoenixes.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pagePhoenixList(HTTP::Connection& connection, HTML::Instance& instance)
{
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

                        tableDataCell.plain("Bezeichnung");
                    }

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

                        tableDataCell.plain("Notifications");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance,
                                HTML::Nothing,
                                HTML::Nothing,
                                3);
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

                        tableDataCell.plain(phoenix.description);
                    }

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
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "value");

                        tableDataCell.plain("%lu", phoenix.numberOfNotifications());
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

                            url.image("img/edit.png", "Bearbeiten");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionPhoenixRemove.c_str(),
                                    WWW::PhoenixId.c_str(),
                                    std::to_string(phoenix.phoenixId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Unwiderruflich aus dem System entfernen.");

                            url.image("img/delete.png", "Löschen");

                            url.plain("[Löschen]");
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
            "colloquium",
            "colloquium",
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

/**
 * @brief   Generate HTML page for the 'Phoenix' remove form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::generatePhoenixRemoveForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long phoenixId;

    try
    {
        phoenixId = connection[WWW::PhoenixId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        instance.alertMessage("Fehler in Browser!");

        return;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "colloquium",
            "colloquium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionPhoenixRemoveConfirmed);

    form.hidden(WWW::PhoenixId, phoenixId);

    Database::Phoenix& phoenix = Database::Phoenixes::PhoenixById(phoenixId);

    unsigned long numberOfNotifications = phoenix.numberOfNotifications();

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Phoenix <b>%s</b> unwiderruflich aus dem System entfernen",
                    phoenix.description.c_str());
        } // HTML.HeadingText

        {
            HTML::Label label(instance);

            label.plain("Phoenix <b>%s</b> wird aus dem System unwiderruflich entfernt.",
                    phoenix.description.c_str());

            label.breakLine();

            label.plain("Für Aktivierung verwendete Aktivierungscode " \
                    "wird freigelassen und darf für Aktivierung anderer Walker verwendet werden.");

            label.breakLine();

            if (numberOfNotifications > 0)
            {
                label.plain("Diesem Phoenix zugeordnete %lu Push Notifications " \
                        "werden unwiderruflich entfernen.",
                        numberOfNotifications);

                label.breakLine();

                label.plain("Entsprechende Fabulas beliben in Datenbank für History " \
                        "und evetuell spätere Debugging.",
                        numberOfNotifications);
            }
        }
    }

    delete &phoenix;

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "south");

        {
            HTML::Button submitButton(instance,
                    HTML::Nothing,
                    HTML::Nothing,
                    WWW::Button,
                    WWW::ButtonSubmit);

            submitButton.plain("Bestätigen");
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
