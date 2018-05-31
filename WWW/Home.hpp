#pragma once

// System definition files.
//
#include <cstdbool>
#include <string>

// Common definition files.
//
#include "HTTP/Connection.hpp"
#include "HTTP/HTML.hpp"
#include "HTTP/Site.hpp"

namespace WWW
{
    static const std::string PageSystemInformation      = "sysinfo";
    static const std::string PageServus                 = "servus";
    static const std::string PagePhoenix                = "phoenix";
    static const std::string PageActivator              = "activator";
    static const std::string PageTherma                 = "therma";

    static const std::string Images                     = "img";
    static const std::string Download                   = "download";
    static const std::string DownloadSubject            = "subject";
    static const std::string DownloadSubjectAjax        = "ajax.js";

    static const std::string Action                     = "action";
    static const std::string ActionLogin                = "login";
    static const std::string ActionServusToggleEnabled  = "servus_toggle";
    static const std::string ActionServusEdit           = "servus_edit";
    static const std::string ActionServusSave           = "servus_save";
    static const std::string ActionPhoenixEdit          = "phoenix_edit";
    static const std::string ActionPhoenixSave          = "phoenix_save";
    static const std::string ActionActivatorEdit        = "activator_edit";
    static const std::string ActionActivatorSave        = "activator_save";
    static const std::string ActionThermaEdit           = "therma_edit";
    static const std::string ActionThermaSave           = "therma_save";

    static const std::string Button                     = "button";
    static const std::string ButtonSubmit               = "submit";
    static const std::string ButtonCancel               = "cancel";

    static const std::string Username                   = "username";
    static const std::string Password                   = "password";

    static const std::string ServusId                   = "servus_id";
    static const std::string ServusDescription          = "servus_name";

    static const std::string PhoenixId                  = "phoenix_id";
    static const std::string PhoenixDescription         = "phoenix_name";

    static const std::string ActivatorId                = "activator_id";
    static const std::string ActivationCode             = "activation_code";
    static const std::string ActivatorDescription       = "activator_name";

    static const std::string ThermaId                   = "therma_id";
    static const std::string ThermaDescription          = "therma_name";

    class Site : public HTTP::Site
    {
    public:
        void
        generateDocument(HTTP::Connection&);

    private:
        /**
         * @brief   Generate the north (upper) part of main page.
         *
         * @param[in]   connection  Pointer to HTTP connection.
         * @param[in]   instance    Pointer to HTML instance.
         */
        void
        pageNorth(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate the south (lower) part of main page.
         *
         * @param[in]   connection  Pointer to HTTP connection.
         * @param[in]   instance    Pointer to HTML instance.
         */
        void
        pageSouth(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate login form.
         *
         * @param[in]   connection  Pointer to HTTP connection.
         * @param[in]   instance    Pointer to HTML instance.
         */
        void
        pageLogin(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'System Information' tab.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageSystemInformation(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Servus' tab.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageServus(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Show list of Servuses.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageServusList(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Servus' edit form.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageServusEditForm(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Phoenix' tab.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pagePhoenix(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Phoenix' edit form.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pagePhoenixEditForm(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Activator' tab.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageActivator(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Activator' edit form.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageActivatorEditForm(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Therma' tab.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageTherma(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Generate HTML page for the 'Therma' edit form.
         *
         * @param   connection      HTTP connection.
         * @param   instance        HTML instance.
         */
        void
        pageThermaEditForm(HTTP::Connection&, HTML::Instance&);

        /**
         * @brief   Check whether some HTML form has been submitted by user.
         *
         * @param[in]   connection  Pointer to HTTP connection.
         *
         * @return  Boolean true if a form has been submitted by user.
         * @return  Boolean false if current HTTP request is regular request without any submit.
         */
        bool
        formSubmitted(HTTP::Connection&);

        /**
         * @brief   Check whether HTML form has been cancelled by user.
         *
         * @param[in]   connection  Pointer to HTTP connection.
         *
         * @return  Boolean true if a form has been cancelled by user.
         * @return  Boolean false if a form has not been cancelled by user.
         */
        bool
        formCancelled(HTTP::Connection&);
    };
};
