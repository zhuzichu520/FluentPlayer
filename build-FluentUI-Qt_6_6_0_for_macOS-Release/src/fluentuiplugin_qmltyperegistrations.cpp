/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#include <Def.h>
#include <FluApp.h>
#include <FluCaptcha.h>
#include <FluColorSet.h>
#include <FluColors.h>
#include <FluEventBus.h>
#include <FluFramelessHelper.h>
#include <FluNetwork.h>
#include <FluRectangle.h>
#include <FluTextStyle.h>
#include <FluTheme.h>
#include <FluTools.h>
#include <FluTreeModel.h>
#include <FluViewModel.h>
#include <FluWatermark.h>
#include <QRCode.h>
#include <Screenshot.h>
#include <WindowLifecycle.h>


#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_FluentUI()
{
    qmlRegisterTypesAndRevisions<FluApp>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluCalendarViewType::staticMetaObject;},
            "FluCalendarViewType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluCalendarViewType::staticMetaObject, "FluentUI", 1, nullptr, &FluCalendarViewType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluCaptcha>("FluentUI", 1);
    qmlRegisterAnonymousType<QQuickItem, 254>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluColorSet>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluColors>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluContentDialogType::staticMetaObject;},
            "FluContentDialogType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluContentDialogType::staticMetaObject, "FluentUI", 1, nullptr, &FluContentDialogType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluEvent>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluEventBus>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluFramelessHelper>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluNavigationViewType::staticMetaObject;},
            "FluNavigationViewType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluNavigationViewType::staticMetaObject, "FluentUI", 1, nullptr, &FluNavigationViewType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluNetwork>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluNetworkType::staticMetaObject;},
            "FluNetworkType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluNetworkType::staticMetaObject, "FluentUI", 1, nullptr, &FluNetworkType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluPageType::staticMetaObject;},
            "FluPageType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluPageType::staticMetaObject, "FluentUI", 1, nullptr, &FluPageType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluRectangle>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluScreenshotType::staticMetaObject;},
            "FluScreenshotType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluScreenshotType::staticMetaObject, "FluentUI", 1, nullptr, &FluScreenshotType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluStatusViewType::staticMetaObject;},
            "FluStatusViewType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluStatusViewType::staticMetaObject, "FluentUI", 1, nullptr, &FluStatusViewType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluTabViewType::staticMetaObject;},
            "FluTabViewType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluTabViewType::staticMetaObject, "FluentUI", 1, nullptr, &FluTabViewType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluTextStyle>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluTheme>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluThemeType::staticMetaObject;},
            "FluThemeType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluThemeType::staticMetaObject, "FluentUI", 1, nullptr, &FluThemeType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluTimePickerType::staticMetaObject;},
            "FluTimePickerType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluTimePickerType::staticMetaObject, "FluentUI", 1, nullptr, &FluTimePickerType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluTimelineType::staticMetaObject;},
            "FluTimelineType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluTimelineType::staticMetaObject, "FluentUI", 1, nullptr, &FluTimelineType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluTools>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<FluTreeModel>("FluentUI", 1);
    qmlRegisterAnonymousType<QAbstractItemModel, 254>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluTreeViewType::staticMetaObject;},
            "FluTreeViewType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluTreeViewType::staticMetaObject, "FluentUI", 1, nullptr, &FluTreeViewType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluViewModel>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluViewModelType::staticMetaObject;},
            "FluViewModelType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluViewModelType::staticMetaObject, "FluentUI", 1, nullptr, &FluViewModelType::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<FluWatermark>("FluentUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &FluWindowType::staticMetaObject;},
            "FluWindowType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&FluWindowType::staticMetaObject, "FluentUI", 1, nullptr, &FluWindowType::staticMetaObject, nullptr);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &Fluent_Awesome::staticMetaObject;},
            "Fluent_Awesome");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&Fluent_Awesome::staticMetaObject, "FluentUI", 1, nullptr, &Fluent_Awesome::staticMetaObject, nullptr);
    qmlRegisterTypesAndRevisions<NetworkCallable>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<NetworkParams>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<QRCode>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<Screenshot>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<ScreenshotBackground>("FluentUI", 1);
    qmlRegisterTypesAndRevisions<WindowLifecycle>("FluentUI", 1);
    qmlRegisterModule("FluentUI", 1, 0);
}

static const QQmlModuleRegistration registration("FluentUI", qml_register_types_FluentUI);
