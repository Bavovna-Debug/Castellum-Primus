// System definition files.
//
#include <cstdbool>
#include <stdexcept>
#include <string>

// Common definition files.
//
#include "HTTP/Connection.hpp"
#include "HTTP/HTML.hpp"
#include "HTTP/HTTP.hpp"
#include "HTTP/Site.hpp"

// Local definition files.
//
#include "Primus/Kernel.hpp"
#include "Primus/Database/Relay.hpp"
#include "Primus/Database/Relays.hpp"
#include "Primus/WWW/Home.hpp"
#include "Primus/WWW/SessionManager.hpp"

/**
 * @brief   Generate HTML main page.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 */
void
WWW::Site::generateDocument(HTTP::Connection& connection)
{
    bool loginPermitted = false;

    {
        WWW::SessionManager& sessionManager = WWW::SessionManager::SharedInstance();

        if (connection.argumentPairExists(WWW::Action, WWW::ActionLogin) == true)
        {
            sessionManager.login(connection.remoteAddress, connection[WWW::Password]);
        }

        if (sessionManager.permitted(connection.remoteAddress) == true)
        {
            loginPermitted = true;
        }
    }

    HTML::Instance instance(connection);

    this->processRelays(connection);

    if (connection.pageName().find(WWW::JavaScript) == 0)
    {
        connection.download(Workspace::RootPath + connection.pageName());
    }
    else if (connection.pageName().find(WWW::Images) == 0)
    {
        connection.download(Workspace::RootPath + connection.pageName());
    }
    else
    { // HTML.Document
        HTML::Document document(instance);

        { // HTML.Head
            HTML::Head head(instance);

            { // HTML::Title
                HTML::Title title(instance);

                title.plain("Primus");
            } // HTML::Title

            head.meta("Content-Type", "text/html; charset=utf-8");

            head.styleSheet("layout.css", "text/css", "screen,projection");
            head.styleSheet("tabs.css", "text/css");
            head.styleSheet("messages.css", "text/css");
            head.styleSheet("workspace.css", "text/css");
            head.styleSheet("form.css", "text/css");

            {
                char ajaxURL[200];

                snprintf(ajaxURL, sizeof(ajaxURL) - sizeof(char),
                        "%s?%s=%s",
                        WWW::Download.c_str(),
                        WWW::DownloadSubject.c_str(),
                        WWW::DownloadSubjectAjax.c_str());

                HTML::Script script(instance, "text/javascript", ajaxURL);
            }
        } // HTML.Head

        { // HTML.Body
            HTML::Body body(instance);

            if (loginPermitted == false)
            {
                // Login form.
                //
                { // HTML.Division
                    HTML::Division division(instance, "content-south", HTML::Nothing);

                    // Contents should be located inside of inliner, which defines fixed location on a page.
                    //
                    { // HTML.Division
                        HTML::Division division(instance, HTML::Nothing, "inliner");

                        this->pageLogin(connection, instance);
                    } // HTML.Division
                } // HTML.Division
            }
            else
            {
                // Static upper part of document.
                //
                { // HTML.Division
                    HTML::Division division(instance, "content-north", HTML::Nothing);

                    // Contents should be located inside of inliner, which defines fixed location on a page.
                    //
                    { // HTML.Division
                        HTML::Division division(instance, HTML::Nothing, "inliner");

                        this->pageNorth(connection, instance);
                    } // HTML.Division
                } // HTML.Division

                // Static lower part of document.
                //
                { // HTML.Division
                    HTML::Division division(instance, "content-south", HTML::Nothing);

                    // Contents should be located inside of inliner, which defines fixed location on a page.
                    //
                    { // HTML.Division
                        HTML::Division division(instance, HTML::Nothing, "inliner");

                        this->pageSouth(connection, instance);
                    } // HTML.Division
                } // HTML.Division
            }
        } // HTML.Body
    } // HTML.Document
}

void
WWW::Site::processRelays(HTTP::Connection& connection)
{
    try
    {
        const unsigned long relayId = connection[WWW::SwitchRelay];

        Database::Relay& relay = Database::Relays::RelayById(relayId);

        try
        {
            const std::string relayState = connection[WWW::RelayState];

            if (relayState == WWW::RelayStateDown)
            {
                relay.switchOff();
            }
            else if (relayState == WWW::RelayStateUp)
            {
                relay.switchOn();
            }
        }
        catch (HTTP::ArgumentDoesNotExist&)
        {
            relay.switchOver();
        }

        delete &relay;
    }
    catch (HTTP::ArgumentDoesNotExist&)
    { }
}

/**
 * @brief   Generate the north (upper) part of main page.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 * @param[in]   instance    Pointer to HTML instance.
 */
void
WWW::Site::pageNorth(HTTP::Connection& connection, HTML::Instance& instance)
{
    { // HTML.Division
        HTML::Division division(instance, HTML::Nothing, "tabs");

        // Tabs line.
        //
        { // HTML.UnorderedList
            HTML::UnorderedList unorderedList(instance, HTML::Nothing, "tabs_list");

            // 'Start' tab.
            //
            { // HTML.ListItem
                HTML::ListItem listItem(instance,
                        HTML::Nothing,
                        ((connection.pageName().empty() == true) ||
                        (connection.pageName() == WWW::PageSystemInformation))
                                ? "tabs_item active"
                                : "tabs_item");

                { // HTML.URL
                    HTML::URL url(instance, WWW::PageSystemInformation);

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "title");

                        span.plain("Start");
                    } // HTML.Span

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "subtitle");

                        span.plain("Systeminformation");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.ListItem

            // 'Servus' tab.
            //
            { // HTML.ListItem
                HTML::ListItem listItem(instance,
                        HTML::Nothing,
                        (connection.pageName() == WWW::PageServus)
                                ? "tabs_item active"
                                : "tabs_item");

                { // HTML.URL
                    HTML::URL url(instance, WWW::PageServus);

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "title");

                        span.plain("Servus");
                    } // HTML.Span

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "subtitle");

                        span.plain("Servus Systeme");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.ListItem

            // 'Phoenix' tab.
            //
            { // HTML.ListItem
                HTML::ListItem listItem(instance,
                        HTML::Nothing,
                        (connection.pageName() == WWW::PagePhoenix)
                                ? "tabs_item active"
                                : "tabs_item");

                { // HTML.URL
                    HTML::URL url(instance, WWW::PagePhoenix);

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "title");

                        span.plain("Phoenix");
                    } // HTML.Span

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "subtitle");

                        span.plain("Mobile devices");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.ListItem

            // 'Activator' tab.
            //
            { // HTML.ListItem
                HTML::ListItem listItem(instance,
                        HTML::Nothing,
                        (connection.pageName() == WWW::PageRelay)
                                ? "tabs_item active"
                                : "tabs_item");

                { // HTML.URL
                    HTML::URL url(instance, WWW::PageRelay);

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "title");

                        span.plain("Relais");
                    } // HTML.Span

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "subtitle");

                        span.plain("Relaisstation");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.ListItem

            // 'Therma' tab.
            //
            { // HTML.ListItem
                HTML::ListItem listItem(instance,
                        HTML::Nothing,
                        (connection.pageName() == WWW::PageTherma)
                                ? "tabs_item active"
                                : "tabs_item");

                { // HTML.URL
                    HTML::URL url(instance, WWW::PageTherma);

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "title");

                        span.plain("Therma");
                    } // HTML.Span

                    { // HTML.Span
                        HTML::Span span(instance, HTML::Nothing, "subtitle");

                        span.plain("Thermalzone");
                    } // HTML.Span
                } // HTML.URL
            } // HTML.ListItem
        } // HTML.UnorderedList
    } // HTML.Division
}

/**
 * @brief   Generate the south (lower) part of main page.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 * @param[in]   instance    Pointer to HTML instance.
 */
void
WWW::Site::pageSouth(HTTP::Connection& connection, HTML::Instance& instance)
{
    if (connection.pageName().empty() == true)
    {
        this->pageSystemInformation(connection, instance);
    }
    else if (connection.pageName() == WWW::PageSystemInformation)
    {
        this->pageSystemInformation(connection, instance);
    }
    else if (connection.pageName() == WWW::PageServus)
    {
        this->pageServus(connection, instance);
    }
    else if (connection.pageName() == WWW::PagePhoenix)
    {
        this->pagePhoenix(connection, instance);
        this->pageActivator(connection, instance);
    }
    else if (connection.pageName() == WWW::PageRelay)
    {
        this->pageRelay(connection, instance);
    }
    else if (connection.pageName() == WWW::PageTherma)
    {
        this->pageTherma(connection, instance);
    }
    else
    {
        this->pageSystemInformation(connection, instance);
    }
}

/**
 * @brief   Generate login form.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 * @param[in]   instance    Pointer to HTML instance.
 */
void
WWW::Site::pageLogin(HTTP::Connection& connection, HTML::Instance& instance)
{
    HTML::Form form(instance,
            HTML::Get,
            "full",
            "colloquium",
            "colloquium",
            connection.pageName());

    form.hidden(WWW::Action, WWW::ActionLogin);

    {
        HTML::FieldSet fieldSet(instance, HTML::Nothing, "north");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Anmeldung");
        } // HTML.HeadingText

        {
            HTML::DefinitionList definitionList(instance);

            {
                HTML::DefinitionTerm definitionTerm(instance);

                {
                    HTML::Label label(instance);

                    label.plain("Benutzername");
                }
            }

            {
                HTML::DefinitionDescription definitionDescription(instance);

                form.textField("description", "inputbox",
                        WWW::Username,
                        "",
                        20, 20);
            }
        }

        {
            HTML::DefinitionList definitionList(instance);

            {
                HTML::DefinitionTerm definitionTerm(instance);

                {
                    HTML::Label label(instance);

                    label.plain("Passwort");
                }
            }

            {
                HTML::DefinitionDescription definitionDescription(instance);

                form.password("description", "inputbox",
                        WWW::Password,
                        "",
                        20, 20);
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

            submitButton.plain("Login");
        }
    }
}

/**
 * @brief   Check whether some HTML form has been submitted by user.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 *
 * @return  Boolean true if a form has been submitted by user.
 * @return  Boolean false if current HTTP request is regular request without any submit.
 */
bool
WWW::Site::formSubmitted(HTTP::Connection& connection)
{
    return connection.argumentPairExists(WWW::Button, WWW::ButtonSubmit);
}

/**
 * @brief   Check whether HTML form has been cancelled by user.
 *
 * @param[in]   connection  Pointer to HTTP connection.
 *
 * @return  Boolean true if a form has been cancelled by user.
 * @return  Boolean false if a form has not been cancelled by user.
 */
bool
WWW::Site::formCancelled(HTTP::Connection& connection)
{
    return connection.argumentPairExists(WWW::Button, WWW::ButtonCancel);
}
