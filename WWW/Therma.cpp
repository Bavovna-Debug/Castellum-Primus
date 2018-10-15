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
#include "Primus/Database/DHTSensor.hpp"
#include "Primus/Database/DHTSensorList.hpp"
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Servuses.hpp"
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
                        const std::string thermaTitle = connection[WWW::ThermaTitle];
                        if (thermaTitle.empty() == true)
                            throw HTTP::ArgumentDoesNotExist();

                        Database::Therma& therma = Database::Thermas::SensorById(thermaId);

                        therma.setTitle(thermaTitle);

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

    this->pageThermaDiagram(connection, instance);
    this->pageThermaDatasheet(connection, instance);
}

void
WWW::Site::pageThermaDatasheet(HTTP::Connection& connection, HTML::Instance& instance)
{
    unsigned long numberOfDHTSensors = Database::DHTSensorList::TotalNumber();
    unsigned long numberOfDSSensors = Database::Thermas::TotalNumber();

    if ((numberOfDHTSensors > 0) && (numberOfDSSensors > 0))
    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Sensoren DHT11/DHT22 und DS18B20/DS18S20");
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
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        tableDataCell.plain("Servus");
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
                        HTML::TableDataCell tableDataCell(instance,
                                HTML::Nothing,
                                HTML::Nothing,
                                2);
                    }
                }
            }

            {
                HTML::TableBody tableBody(instance);

                for (unsigned int sensorIndex = 0;
                     sensorIndex < numberOfDHTSensors;
                     sensorIndex++)
                {
                    Database::DHTSensor& sensor = Database::DHTSensorList::SensorByIndex(sensorIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(sensor.title);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        Database::Servus& servus = Database::Servuses::ServusById(sensor.servusId);

                        tableDataCell.plain(servus.title);

                        delete &servus;
                    }

                    float lastKnownHumidity = sensor.lastKnownHumidity();
                    float lowestKnownHumidity = sensor.lowestKnownHumidity();
                    float highestKnownHumidity = sensor.highestKnownHumidity();

                    float lastKnownTemperature = sensor.lastKnownTemperature();
                    float lowestKnownTemperature = sensor.lowestKnownTemperature();
                    float highestKnownTemperature = sensor.highestKnownTemperature();

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("%4.2f &#x2103; <br /> %4.2f %%",
                                lowestKnownTemperature,
                                lowestKnownHumidity);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("[-%4.2f &#x2103;] <br /> [%4.2f %%]",
                                lastKnownTemperature - lowestKnownTemperature,
                                lastKnownHumidity - lowestKnownHumidity);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "green");

                        tableDataCell.plain("%4.2f &#x2103; <br /> %4.2f %%",
                                lastKnownTemperature,
                                lastKnownHumidity);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("[+%4.2f &#x2103;] <br /> [%4.2f %%]",
                                highestKnownTemperature - lastKnownTemperature,
                                highestKnownHumidity - lastKnownHumidity);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("%4.2f &#x2103; <br /> %4.2f %%",
                                highestKnownTemperature,
                                highestKnownHumidity);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "value");

                        tableDataCell.plain("%3.2f &#x2103; <br /> %3.2f %%",
                                sensor.temperatureEdge,
                                sensor.humidityEdge);
                    }

                    delete &sensor;
                }

                for (unsigned int sensorIndex = 0;
                     sensorIndex < numberOfDSSensors;
                     sensorIndex++)
                {
                    Database::Therma& sensor = Database::Thermas::SensorByIndex(sensorIndex);

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "label");

                        tableDataCell.plain(sensor.title);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "centered");

                        Database::Servus& servus = Database::Servuses::ServusById(sensor.servusId);

                        tableDataCell.plain(servus.title);

                        delete &servus;
                    }

                    float lastKnownTemperature = sensor.lastKnownTemperature();
                    float lowestKnownTemperature = sensor.lowestKnownTemperature();
                    float highestKnownTemperature = sensor.highestKnownTemperature();

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("%4.2f &#x2103;",
                                lowestKnownTemperature);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "blue");

                        tableDataCell.plain("[-%4.2f &#x2103;]",
                                lastKnownTemperature - lowestKnownTemperature);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "green");

                        tableDataCell.plain("%4.2f &#x2103;",
                                lastKnownTemperature);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("[+%4.2f &#x2103;]",
                                highestKnownTemperature - lastKnownTemperature);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "red");

                        tableDataCell.plain("%4.2f &#x2103;",
                                highestKnownTemperature);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, HTML::Nothing, "value");

                        tableDataCell.plain("%3.2f &#x2103;",
                                sensor.temperatureEdge);
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
                                    std::to_string(sensor.thermaId).c_str());

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
                                    WWW::ActionThermaDiagram.c_str(),
                                    WWW::ThermaId.c_str(),
                                    std::to_string(sensor.thermaId).c_str());

                            HTML::URL url(instance,
                                    urlString,
                                    "Temperatur Diagramm.");

                            url.image("img/diagram.png", "Diagramm");

                            url.plain("[Diagramm]");
                        } // HTML.URL
                    }

                    delete &sensor;
                }
            }
        }
    } // HTML.Division
}

void
WWW::Site::pageThermaDiagram(HTTP::Connection& connection, HTML::Instance& instance)
{
    if (connection.argumentPairExists(WWW::Action, WWW::ActionThermaDiagram) == true)
    {
        try
        {
            const unsigned long thermaId = connection[WWW::ThermaId];

            HTML::Division division(instance, HTML::Nothing, "workspace");

            { // HTML.Division
                HTML::Division division(instance, "full", "slice");

                { // HTML.HeadingText
                    HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

                    headingText.plain("Diagram");
                } // HTML.HeadingText

                {
                    HTML::Division division(instance, "Diagram", HTML::Nothing);
                }

                {
                    HTML::Script script(instance, "text/javascript", "js/chart.js");
                }

                {
                    HTML::Script script(instance, "text/javascript", "js/line-chart.js");
                }

                {
                    HTML::Script script(instance, "text/javascript", HTML::Nothing);

                    Database::Therma& therma = Database::Thermas::SensorById(thermaId);

                    script.plain(therma.diagramAsJava());
                    script.plain("new LineChart(document.getElementById('Diagram'), data);");

                    delete &therma;
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
