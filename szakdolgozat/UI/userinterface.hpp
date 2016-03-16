#pragma once

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSizePolicy> 
#include <QVBoxLayout>
#include "../../GeneratedFiles/ui_userinterface.h"
#include "request.h"

class UserInterface : public QWidget
{
	Q_OBJECT

public:
	UserInterface(QWidget *parent = 0);
	~UserInterface();
	Request GetRequest();

protected:
	void Init();
	void ResetRequest();
	void AddItemsToDropMenu();
	void SetLabelProperties();
	void SetButtonsProperties();
	void SetInputLineProperties();
	void CreateHead(QVBoxLayout*);
	void CreateRadios(QHBoxLayout*);
	void CreateMiddleButtons(QHBoxLayout*);
	void CreateMiddle(QVBoxLayout*, QHBoxLayout*, QHBoxLayout*);
	void CreatePlaneDetails(QVBoxLayout*);
	void CreateBottom(QHBoxLayout*);
	
private slots:
	void radio_handler();
	void cuttingEvent();
	void undoEvent();
	void acceptEvent();
	void restartEvent();
	void infoEvent();
	void backToMenu();
	void typeaccept_handler();
	void newplane_event();

private:
	QWidget *window;
	QVBoxLayout* _mainLayout;
	QLabel * _label, *_info;
	QLabel * _x, *_y, *_z;
	QLineEdit * _xf, *_yf, *_zf;
	QLineEdit * _xn, *_yn, *_zn;
	QRadioButton* _user;
	QRadioButton* _autom;
	QPushButton* _cutting;
	QPushButton* _undo;
	QPushButton* _accept;
	QPushButton* _restart;
	QPushButton* _moreInfo;
	QPushButton* _back;
	QComboBox* _dropDownMenu;
	QComboBox* _accepttypes;

	Request request;

};
