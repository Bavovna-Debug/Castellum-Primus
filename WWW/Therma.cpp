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
#include "Primus/Database/Therma.hpp"
#include "Primus/Database/Thermas.hpp"
#include "Primus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Therma' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageTherma(HTTP::Connection& connection, HTML::Instance& instance)
{
    // Process forms.
    //
    {
        if (connection.argumentPairExists(WWW::Action, WWW::ActionThermaEdit) == true)
        {
            this->pageThermaEditForm(connection, instance);

            return;
        }

        if (this->formSubmitted(connection) == true)
        {
            if (connection.argumentPairExists(WWW::Action, WWW::ActionThermaSave) == true)
            {
                try
                {
                    const unsigned long thermaId = connection[WWW::ThermaId];

                    try
                    {
                        const std::string thermaDescription = connection[WWW::ThermaDescription];
                        if (thermaDescription.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        Database::Therma& therma = Database::Thermas::ThermaById(thermaId);

                        therma.setDescription(thermaDescription);

                        delete &therma;
                    }
                    catch (HTTP::ArgumentDoesNotExist&)
                    {
                        instance.errorMessage("Beschreibung fehlt!");

                        this->pageThermaEditForm(connection, instance);

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

            headingText.plain("Therma");
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

                        tableDataCell.plain("GPIO Id");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Tief");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Delta");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Aktuell");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Delta");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Hoch");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Präzision");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);
                    }
                }
            }

            {
                HTML::TableBody tableBody(instance);

                unsigned long numberOfThermas = Database::Thermas::TotalNumber();
                for (unsigned int thermaIndex = 0;
                     thermaIndex < numberOfThermas;
                     thermaIndex++)
                {
                    Database::Therma& therma = Database::Thermas::ThermaByIndex(thermaIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(therma.description);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "dump");

                        tableDataCell.plain(therma.gpioDeviceNumber);
                    }

                    float current = therma.lastKnownTemperature();
                    float lowest = therma.lowestKnownTemperature();
                    float highest = therma.highestKnownTemperature();

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("%4.2f &#x2103;", lowest);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("[-%4.2f &#x2103;]", current - lowest);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "green");

                        tableDataCell.plain("%4.2f &#x2103;", current);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("[+%4.2f &#x2103;]", highest - current);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("%4.2f &#x2103;", highest);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "value");

                        tableDataCell.plain("%3.2f &#x2103;", therma.edge);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%s&%s=%s",
                                    connection.pageName().c_str(),
                                    WWW::Action.c_str(),
                                    WWW::ActionThermaEdit.c_str(),
                                    WWW::ThermaId.c_str(),
                                    std::to_string(therma.thermaId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Bearbeiten.");

                            url.image("img/edit.png", "Bearbeiten");

                            url.plain("[Bearbeiten]");
                        } // HTML.URL
                    }

                    delete &therma;
                }
            }
        }
    } // HTML.Division
}

/**
 * @brief   Generate HTML page for the 'Therma' edit form.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::pageThermaEditForm(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long thermaId;

    try
    {
        thermaId = connection[WWW::ThermaId];
    }
    catch (HTTP::ArgumentDoesNotExist&)
    {
        thermaId = 0;
    }

    HTML::Form form(instance,
            HTML::Get,
            "full",
            "observatorium",
            "observatorium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionThermaSave);

    form.hidden(WWW::ThermaId, thermaId);

    std::string thermaDescription;

    if (thermaId != 0)
    {
        Database::Therma& therma = Database::Thermas::ThermaById(thermaId);

        thermaDescription = therma.description;

        delete &therma;
    }

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            if (thermaId == 0)
            {
                instance.alertMessage("Fehler in Browser!");

                return;
            }

            headingText.plain("Therma <b>%s</b> bearbeiten", thermaDescription.c_str());
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
                        WWW::ThermaDescription,
                        thermaDescription.c_str(),
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
