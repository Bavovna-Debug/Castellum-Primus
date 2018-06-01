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
#include "Primus/Configuration.hpp"
#include "Primus/Database/Activator.hpp"
#include "Primus/Database/Activators.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Activator' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageActivator(HTTP::Connection& connection, HTML::Instance& instance)
{
    // Process forms.
    //
    {
        if (connection.argumentPairExists(WWW::Action, WWW::ActionActivatorEdit) == true)
        {
            this->pageActivatorEditForm(connection, instance);

            return;
        }

        if (this->formSubmitted(connection) == true)
        {
            if (connection.argumentPairExists(WWW::Action, WWW::ActionActivatorSave) == true)
            {
                try
                {
                    try
                    {
                        const unsigned long activatorId = connection[WWW::ActivatorId];

                        try
                        {
                            const std::string activationCode =
                                    connection[WWW::ActivationCode];
                            const std::string activatorDescription =
                                    connection[WWW::ActivatorDescription];

                            if (activationCode.length() != Primus::ActivationCodeLength)
                            {
                                instance.errorMessage("Aktivierungscode konnte nicht definiert werden!");

                                instance.noticeMessage("Aktivierungscode muss 10 Symbole lang sein!");

                                this->pageActivatorEditForm(connection, instance);

                                return;
                            }

                            Database::Activator& activator =
                                    Database::Activators::ActivatorById(activatorId);

                            activator.setActivationCode(activationCode);
                            activator.setDescription(activatorDescription);

                            delete &activator;
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
                            const std::string activationCode =
                                    connection[WWW::ActivationCode];
                            const std::string activatorDescription =
                                    connection[WWW::ActivatorDescription];

                            const unsigned long activatorId = Database::Activator::DefineActivator(
                                    connection[WWW::ActivationCode],
                                    connection[WWW::ActivatorDescription]);

                            if (activatorId != 0)
                            {
                                instance.successMessage("Neuer Aktivierungscode wurde definiert.");
                            }
                            else
                            {
                                instance.errorMessage("Aktivierungscode konnte nicht definiert werden!");
                            }
                        }
                        catch (HTTP::ArgumentDoesNotExist&)
                        {
                            instance.errorMessage("Aktivierungscode konnte nicht definiert werden!");

                            instance.noticeMessage("Aktivierungscode oder Beschreibung fehlt!");

                            this->pageActivatorEditForm(connection, instance);

                            return;
                        }
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

            headingText.plain("Activators");
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
                            WWW::ActionActivatorEdit.c_str());

                    HTML::URL url(instance,
                            urlString,
                            "Neuen Aktivierungscode erstellen.");

                    url.image("img/new.png", "Neuen Aktivierungscode erstellen.");

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

                        tableDataCell.plain("Bezeichnung");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Definiert");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Zuordnung");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Aktivierungscode");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }
                }
            }

            {
                HTML::TableBody tableBody(instance);

                unsigned long numberOfActivators = Database::Activators::TotalNumber();
                for (unsigned int activatorIndex = 0;
                     activatorIndex < numberOfActivators;
                     activatorIndex++)
                {
                    Database::Activator& activator =
                            Database::Activators::ActivatorByIndex(activatorIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(activator.description);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(activator.timestamp->YYYYMMDDHHMM());
                    }

                    if (activator.phoenixId == 0)
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "shadowed");

                        tableDataCell.plain("Nicht verwendet");
                    }
                    else
                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        Database::Phoenix& phoenix =
                                Database::Phoenixes::PhoenixById(activator.phoenixId);

                        tableDataCell.plain("Seit %s bei ",
                                phoenix.timestamp->YYYYMMDDHHMM().c_str());
                        tableDataCell.setTextStyle(HTML::Bold);
                        tableDataCell.plain(phoenix.description.c_str());
                        tableDataCell.setTextStyle();

                        delete &phoenix;
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "dump");

                        tableDataCell.plain(activator.activationCode);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionActivatorEdit.c_str(),
                                    WWW::ActivatorId.c_str(),
                                    std::to_string(activator.activatorId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Bearbeiten.");

                            url.image("img/edit.png", "Bearbeiten.");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    delete &activator;
                }
            }
        }
    } // HTML.Division
}

/**
 * @brief   Generate HTML page for the 'Activator' edit form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageActivatorEditForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long activatorId;

    try
    {
        activatorId = connection[WWW::ActivatorId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        activatorId = 0;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "observatorium",
            "observatorium", connection.pageName());

    form.hidden(WWW::Action, WWW::ActionActivatorSave);

    if (activatorId != 0)
    {
        form.hidden(WWW::ActivatorId, activatorId);
    }

    std::string activationCode;
    std::string activatorDescription;

    if (activatorId != 0)
    {
        Database::Activator& activator = Database::Activators::ActivatorById(activatorId);

        activationCode = activator.activationCode;
        activatorDescription = activator.description;

        delete &activator;
    }
    else
    {
        try
        {
            const std::string activationCodeFromURL = connection[WWW::ActivationCode];
            const std::string activatorDescriptionFromURL = connection[WWW::ActivatorDescription];

            activationCode = activationCodeFromURL;
            activatorDescription = activatorDescriptionFromURL;
        }
        catch (HTTP::ArgumentDoesNotExist&)
        { }
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            if (activatorId == 0)
            {
                headingText.plain("Neuen Aktivierungscode anlegen");
            }
            else
            {
                headingText.plain("Aktivierung bearbeiten");
            }
        } // HTML.HeadingText

        {
            HTML::DefinitionList definitionList(instance);

            {
                HTML::DefinitionTerm definitionTerm(instance);

                {
                    HTML::Label label(instance);

                    label.plain("Aktivierungscode");
                }
            }

            {
                HTML::DefinitionDescription definitionDescription(instance);

                form.textField("description", "inputbox",
                        WWW::ActivationCode,
                        activationCode.c_str(),
                        10, 40);
            }
        }

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
                        WWW::ActivatorDescription,
                        activatorDescription.c_str(),
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
