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
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Servuses.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Servus' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageServus(HTTP::Connection& connection, HTML::Instance& instance)
{
    // Process forms.
    //
    {
        if (connection.argumentPairExists(WWW::Action, WWW::ActionServusToggleEnabled) == true)
        {
            try
            {
                const unsigned long servusId = connection[WWW::ServusId];

                Database::Servus& servus = Database::Servuses::ServusById(servusId);

                servus.toggleEnabledFlag();

                if (servus.enabled == true)
                {
                    instance.infoMessage("Servus <b>%s</b> wurde <u>aktiviert</u>. " \
                            "Logins von diesem Servus werden künftig akzeptiert.",
                            servus.description.c_str());
                }
                else
                {
                    instance.infoMessage("Servus <b>%s</b> wurde <u>deaktiviert</u>. " \
                            "Logins von diesem Servus werden künftig abgelehnt.",
                            servus.description.c_str());
                }

                delete &servus;
            }
            catch (HTTP::ArgumentDoesNotExist&)
            {
                instance.alertMessage("Fehler in Browser!");

                return;
            }
        }

        if (connection.argumentPairExists(WWW::Action, WWW::ActionServusEdit) == true)
        {
            this->pageServusEditForm(connection, instance);

            return;
        }

        if (this->formSubmitted(connection) == true)
        {
            if (connection.argumentPairExists(WWW::Action, WWW::ActionServusSave) == true)
            {
                try
                {
                    const unsigned long servusId = connection[WWW::ServusId];

                    try
                    {
                        const std::string servusDescription = connection[WWW::ServusDescription];
                        if (servusDescription.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        Database::Servus& servus = Database::Servuses::ServusById(servusId);

                        servus.setDescription(servusDescription);

                        delete &servus;
                    }
                    catch (HTTP::ArgumentDoesNotExist&)
                    {
                        instance.alertMessage("Fehler in Browser!");

                        return;
                    }
                }
                catch (HTTP::ArgumentDoesNotExist&)
                {
                    try
                    {
                        const std::string servusDescription = connection[WWW::ServusDescription];
                        if (servusDescription.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        const unsigned long servusId =
                                Database::Servus::DefineServus(servusDescription);

                        if (servusId != 0)
                        {
                            instance.successMessage("Neuer Servus wurde definiert.");
                        }
                        else
                        {
                            instance.errorMessage("Servus konnte nicht definiert werden!");
                        }
                    }
                    catch (HTTP::ArgumentDoesNotExist&)
                    {
                        instance.errorMessage("Servus konnte nicht definiert werden!");

                        instance.noticeMessage("Beschreibung fehlt!");

                        this->pageServusEditForm(connection, instance);

                        return;
                    }
                }
            }
        }
    }

    this->pageServusList(connection, instance);
}

/**
 * @brief   Show list of Servuses.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageServusList(HTTP::Connection& connection, HTML::Instance& instance)
{
    HTML::Division division(instance, HTML::Nothing, "workspace");

    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Servuses");
        } // HTML.HeadingText

        { // HTML.Division
            HTML::Division division(instance, HTML::Nothing, "controls");

            { // HTML.Division
                HTML::Division division(instance, HTML::Nothing, "button");

                { // HTML.URL
                    char urlString[200];

                    snprintf(urlString, sizeof(urlString),
                            "%s?%s=%s",
                            connection.pageName().c_str(),
                            WWW::Action.c_str(),
                            WWW::ActionServusEdit.c_str());

                    HTML::URL url(instance,
                            urlString,
                            "Neuen Servus erstellen.");

                    url.image("img/new.png", "Neuen Servus erstellen.");

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, HTML::Nothing);

                        span.plain("Definieren");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.Division
        } // HTML.Division

        {
            HTML::Table table(instance);

            {
                HTML::TableHeader tableHeader(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Enabled");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Online");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Authenticator");
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

                unsigned long numberOfServuses = Database::Servuses::TotalNumber();
                for (unsigned int servusIndex = 0;
                     servusIndex < numberOfServuses;
                     servusIndex++)
                {
                    Database::Servus& servus =
                            Database::Servuses::ServusByIndex(servusIndex);

                    HTML::TableRow tableRow(instance);

                    if (servus.enabled == true)
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "green");

                        tableDataCell.plain("Enabled");
                    }
                    else
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("Disabled");
                    }

                    if (servus.online == true)
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "green");

                        tableDataCell.plain("Online");
                    }
                    else
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("Offline");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "dump");

                        tableDataCell.plain(servus.authenticator);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(servus.description);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionServusEdit.c_str(),
                                    WWW::ServusId.c_str(),
                                    std::to_string(servus.servusId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Bearbeiten.");

                            url.image("img/edit.png", "Bearbeiten.");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        if (servus.enabled == true)
                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionServusToggleEnabled.c_str(),
                                    WWW::ServusId.c_str(),
                                    std::to_string(servus.servusId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Deaktivieren.");

                            url.image("img/disable.png", "Deaktivieren.");

                            url.plain("[Deaktivieren]");
                        } // HTML.URL
                        else
                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionServusToggleEnabled.c_str(),
                                    WWW::ServusId.c_str(),
                                    std::to_string(servus.servusId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Aktivieren.");

                            url.image("img/enable.png", "Aktivieren.");

                            url.plain("[Aktivieren]");
                        } // HTML.URL
                    }

                    delete &servus;
                }
            }
        }
    } // HTML.Division
}

/**
 * @brief   Generate HTML page for the 'Servus' edit form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageServusEditForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long servusId;

    try
    {
        servusId = connection[WWW::ServusId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        servusId = 0;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "observatorium",
            "observatorium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionServusSave);

    if (servusId != 0)
    {
        form.hidden(WWW::ServusId, servusId);
    }

    std::string servusDescription;

    if (servusId != 0)
    {
        Database::Servus& servus = Database::Servuses::ServusById(servusId);

        servusDescription = servus.description;

        delete &servus;
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            if (servusId == 0)
            {
                headingText.plain("Neuen Servus definieren");
            }
            else
            {
                headingText.plain("Servus bearbeiten");
            }
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
                        WWW::ServusDescription,
                        servusDescription.c_str(),
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
