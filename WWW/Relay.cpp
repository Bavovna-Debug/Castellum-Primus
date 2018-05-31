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
#include "Primus/Database/Relay.hpp"
#include "Primus/Database/Relays.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Relay' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageRelay(HTTP::Connection& connection, HTML::Instance& instance)
{
    // Process forms.
    //
    {
        if (connection.argumentPairExists(WWW::Action, WWW::ActionRelayEdit) == true)
        {
            this->pageRelayEditForm(connection, instance);

            return;
        }

        if (this->formSubmitted(connection) == true)
        {
            if (connection.argumentPairExists(WWW::Action, WWW::ActionRelaySave) == true)
            {
                try
                {
                    const unsigned long relayId = connection[WWW::RelayId];

                    try
                    {
                        const std::string relayDescription = connection[WWW::RelayDescription];
                        if (relayDescription.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        Database::Relay& relay = Database::Relays::RelayById(relayId);

                        relay.setDescription(relayDescription);

                        delete &relay;
                    }
                    catch (HTTP::ArgumentDoesNotExist&)
                    {
                        instance.errorMessage("Beschreibung fehlt!");

                        this->pageRelayEditForm(connection, instance);

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

            headingText.plain("Relay");
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

                        tableDataCell.plain("GPIO Pin");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Status");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }
                }
            }

            {
                HTML::TableBody tableBody(instance);

                unsigned long numberOfRelays = Database::Relays::TotalNumber();
                for (unsigned int relayIndex = 0;
                     relayIndex < numberOfRelays;
                     relayIndex++)
                {
                    Database::Relay& relay = Database::Relays::RelayByIndex(relayIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(relay.description);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain("%u", relay.gpioPinNumber);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing,
                                (relay.isOff()) ? "red" : "green");

                        tableDataCell.plain((relay.isOff()) ? "Aus" : "Ein");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%u&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::SwitchRelay.c_str(),
                                    relayIndex,
                                    WWW::RelayState.c_str(),
                                    WWW::RelayStateUp.c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Schalte Relais ein.");

                            url.image("img/enable.png", "Einschalten.");

                            { // HTML.Span
                                HTML::Span span(instance, HTML::Nothing, HTML::Nothing);

                                span.plain("Ein");
                            } // HTML.Span
                        } // HTML.URL
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%u&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::SwitchRelay.c_str(),
                                    relayIndex,
                                    WWW::RelayState.c_str(),
                                    WWW::RelayStateDown.c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Schalte Relais aus.");

                            url.image("img/disable.png", "Ausschalten.");

                            { // HTML.Span
                                HTML::Span span(instance, HTML::Nothing, HTML::Nothing);

                                span.plain("Aus");
                            } // HTML.Span
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
                                    WWW::ActionRelayEdit.c_str(),
                                    WWW::RelayId.c_str(),
                                    std::to_string(relay.relayId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Bearbeiten.");

                            url.image("img/edit.png", "Bearbeiten.");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    delete &relay;
                }
            }
        }
    } // HTML.Division
}

/**
 * @brief   Generate HTML page for the 'Relay' edit form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageRelayEditForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long relayId;

    try
    {
        relayId = connection[WWW::RelayId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        relayId = 0;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "observatorium",
            "observatorium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionRelaySave);

    form.hidden(WWW::RelayId, relayId);

    std::string relayDescription;

    if (relayId != 0)
    {
        Database::Relay& relay = Database::Relays::RelayById(relayId);

        relayDescription = relay.description;

        delete &relay;
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            if (relayId == 0)
            {
                instance.alertMessage("Fehler in Browser!");

                return;
            }

            headingText.plain("Relay <b>%s</b> bearbeiten", relayDescription.c_str());
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
                        WWW::RelayDescription,
                        relayDescription.c_str(),
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
