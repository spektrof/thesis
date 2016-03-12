/********************************************************************************
** Form generated from reading UI file 'userinterface.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERINTERFACE_H
#define UI_USERINTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserInterfaceClass
{
public:

	void setupUi(QWidget *UserInterfaceClass)
	{
		if (UserInterfaceClass->objectName().isEmpty())
			UserInterfaceClass->setObjectName(QStringLiteral("UserInterfaceClass"));
		UserInterfaceClass->resize(600, 400);

		retranslateUi(UserInterfaceClass);

		QMetaObject::connectSlotsByName(UserInterfaceClass);
	} // setupUi

	void retranslateUi(QWidget *UserInterfaceClass)
	{
		UserInterfaceClass->setWindowTitle(QApplication::translate("UserInterfaceClass", "UserInterface", 0));
	} // retranslateUi

};

namespace Ui {
	class UserInterfaceClass : public Ui_UserInterfaceClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERINTERFACE_H
