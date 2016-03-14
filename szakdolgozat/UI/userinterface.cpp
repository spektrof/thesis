﻿#include "userinterface.hpp"

UserInterface::UserInterface(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this); // -> lehet nem kell az egyik generált cucc
	window = new QWidget();
	_mainLayout = new QVBoxLayout();
	_label = new QLabel("Welcome", this);
	_info = new QLabel("Hey, Im imformation about this ui",this);
	_user = new QRadioButton("User", this);
	_autom = new QRadioButton("Automatic", this);
	_cutting = new QPushButton("Cutting",this);
	_undo = new QPushButton("Undo", this);
	_accept = new QPushButton("Accept", this);
	_restart = new QPushButton("Restart", this);
	_moreInfo = new QPushButton("More Info", this);
	_back = new QPushButton("Back", this);
	_dropDownMenu = new QComboBox();
	_accepttypes = new QComboBox();
	AddItemsToDropMenu();

	Init();
}

UserInterface::~UserInterface()
{
	delete _user;
	delete _autom;
	delete _label;
	delete _info;
	delete _cutting;
	delete _undo;
	delete _accept;
	delete _restart;
	delete _moreInfo;
	delete _back;
	delete _dropDownMenu;
	delete _accepttypes;
	delete window;
	delete _mainLayout;
}

void UserInterface::Init() 
{
	window->setStyleSheet("background-color: #7ac5cd;");
	window->setFixedSize(300, 300);
	window->move(1000, 200);
	//---------------------------------------
	SetLabelProperties();
	SetButtonsProperties();
	//---------------------------------------
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QVBoxLayout *head = new QVBoxLayout;
	QHBoxLayout *radios = new QHBoxLayout;
	QHBoxLayout *middle_buttons = new QHBoxLayout;
	QHBoxLayout *middle_accept = new QHBoxLayout;
	QVBoxLayout *middle = new QVBoxLayout;
	QHBoxLayout *bottom = new QHBoxLayout;
	//------------------------------------------

	CreateHead(head);
	CreateRadios(radios);
	CreateMiddleButtons(middle_buttons);
	CreateMiddle(middle,middle_buttons, middle_accept);
	CreateBottom(bottom);

	//-----------------------------------------	
	mainLayout->addLayout(head);
	mainLayout->addLayout(radios);
	mainLayout->addLayout(middle);
	mainLayout->addLayout(bottom);

	//_----------------------------------------- connects

	connect(_user, SIGNAL(clicked()), this, SLOT(radio_handler()));
	connect(_autom, SIGNAL(clicked()), this, SLOT(radio_handler()));
	connect(_cutting, SIGNAL(clicked()), this, SLOT(cuttingEvent()));
	connect(_undo, SIGNAL(clicked()), this, SLOT(undoEvent()));
	connect(_accept, SIGNAL(clicked()), this, SLOT(acceptEvent()));
	connect(_restart, SIGNAL(clicked()), this, SLOT(restartEvent()));
	connect(_moreInfo, SIGNAL(clicked()), this, SLOT(infoEvent()));
	connect(_back, SIGNAL(clicked()), this, SLOT(backToMenu()));
	connect(_accepttypes, SIGNAL(currentIndexChanged(int)), this, SLOT(typeaccept_handler()));

	//-------------------------------------------

	window->setLayout(mainLayout);
	window->show();
}

void UserInterface::radio_handler()
{
	if (_autom->isChecked()) _dropDownMenu->setVisible(false);
	else					 _dropDownMenu->setVisible(true);
}

void UserInterface::cuttingEvent()
{
	request.happen = CUTTING;
	request.ta = TypeAccept(_accepttypes->currentData().toInt());
	request.uc = _user->isChecked() ? UserControl(_dropDownMenu->currentData().toInt()) : UserControl::AUTOMATIC;

	_cutting->setEnabled(false);
	_undo->setEnabled(true);
	_accept->setEnabled(true);
	_accepttypes->setEnabled(true);
}

void UserInterface::undoEvent()
{
	request.happen = UNDO;
	_accept->setEnabled(false);
	_accepttypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);
}

void UserInterface::acceptEvent()
{
	request.happen = ACCEPT;
	request.ta = TypeAccept(_accepttypes->currentData().toInt());

	_accept->setEnabled(false);
	_accepttypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);
}

void UserInterface::restartEvent()
{
	request.happen = RESTART;
	request.ta = TypeAccept(0);

	_accept->setEnabled(false);
	_accepttypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);
}

void UserInterface::infoEvent()
{
	_info->setVisible(true);
	_back->setVisible(true);
	
	_label->setVisible(false);
	_user->setVisible(false);
	_autom->setVisible(false);
	_cutting->setVisible(false);
	_undo->setVisible(false);
	_accept->setVisible(false);
	_restart->setVisible(false);
	_dropDownMenu->setVisible(false);
	_accepttypes->setVisible(false);
	_moreInfo->setVisible(false);
}

void UserInterface::backToMenu()
{
	_info->setVisible(false);
	_back->setVisible(false);

	_label->setVisible(true);
	_user->setVisible(true);
	_autom->setVisible(true);
	_cutting->setVisible(true);
	_undo->setVisible(true);
	_accept->setVisible(true);
	_restart->setVisible(true);
	_accepttypes->setVisible(true);

	_dropDownMenu->setVisible(false);
	_user->setChecked(false);
	_autom->setChecked(true);

	_moreInfo->setVisible(true);
}

void UserInterface::typeaccept_handler()
{
	request.ta = TypeAccept(_accepttypes->currentData().toInt());
}

Request UserInterface::GetRequest() 
{
	if (request.happen == NONE) return request;

	Request result(request); 

	ResetRequest();
	if (result.happen == ACCEPT)
	{
		request.ta = NEGATIVE;
		_accepttypes->setCurrentIndex(0);
	}
	return result;
}

void UserInterface::ResetRequest() 
{
	request.happen = ToDo::NONE;
}

void UserInterface::SetLabelProperties()
{
	_label->setFont(QFont("Courier New", 16, QFont::Bold));
	//	label->setAlignment(Qt::AlignHCenter);
	_label->setMaximumHeight(25);
}
void UserInterface::SetButtonsProperties() {
	
	_cutting->setFixedSize(80, 30);
	_undo->setFixedSize(80, 30);

	_autom->setChecked(true);

	_accept->setEnabled(false);
	_undo->setEnabled(false);
	_back->setVisible(false);


	_cutting->setStyleSheet("background-color: #bcee68;");
	_undo->setStyleSheet("background-color: #bcee68;");
	_accept->setStyleSheet("background-color: #bcee68;");
	_restart->setStyleSheet("background-color: #bcee68;");
	_moreInfo->setStyleSheet("background-color: #bcee68;");
	_back->setStyleSheet("background-color: #bcee68;");
	_dropDownMenu->setStyleSheet("background-color: #bcee68;");
	_accepttypes->setStyleSheet("background-color: #bcee68;");
}
void UserInterface::CreateHead(QVBoxLayout* head)
{
	head->addWidget(_label, 0, Qt::AlignHCenter);
	head->addWidget(_info, 0, Qt::AlignHCenter);

	head->setSpacing(0);
	head->setMargin(0);

	_info->setVisible(false);
}
void UserInterface::CreateRadios(QHBoxLayout* radios)
{
	radios->addWidget(_user, 0, Qt::AlignCenter | Qt::AlignVCenter);
	radios->addWidget(_autom, 0, Qt::AlignVCenter);

	radios->setSpacing(0);
}
void UserInterface::CreateMiddleButtons(QHBoxLayout* middle_buttons)
{
	middle_buttons->addWidget(_cutting, 0, Qt::AlignVCenter);
	middle_buttons->addWidget(_undo, 0, Qt::AlignVCenter);
	
}
void UserInterface::CreateMiddle(QVBoxLayout* middle, QHBoxLayout* middle_buttons, QHBoxLayout* middle_accept)
{
	if (_autom->isChecked()) _dropDownMenu->setVisible(false);
	else					 _dropDownMenu->setVisible(true);

	//--------------------------
	_dropDownMenu->setFixedHeight(25);
	_accepttypes->setFixedHeight(25);

	_dropDownMenu->setEditable(true);
	_accepttypes->setEditable(true);

	_accepttypes->setEnabled(false);

	_dropDownMenu->lineEdit()->setReadOnly(true);
	_accepttypes->lineEdit()->setReadOnly(true);

	_dropDownMenu->lineEdit()->setAlignment(Qt::AlignCenter);
	_accepttypes->lineEdit()->setAlignment(Qt::AlignCenter);

	for (int i = 0; i < _dropDownMenu->count(); ++i) 
	{
		_dropDownMenu->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _accepttypes->count(); ++i)
	{
		_accepttypes->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}

	middle_accept->addWidget(_accepttypes);
	middle_accept->addWidget(_accept);

	middle->addWidget(_dropDownMenu);
	middle->addLayout(middle_buttons);
	middle->addLayout(middle_accept);

}
void UserInterface::CreateBottom(QHBoxLayout* bottom)
{
	bottom->addWidget(_restart, 0, Qt::AlignBottom | Qt::AlignLeft);
	bottom->addWidget(_moreInfo, 0, Qt::AlignBottom | Qt::AlignRight);
	bottom->addWidget(_back, 0,  Qt::AlignRight);
}

void UserInterface::AddItemsToDropMenu()
{
	_dropDownMenu->addItem("Random plane at the centroid of the atom", 1);
	_dropDownMenu->addItem("Plane lying on a triangle contained in the atom", 2);
	_dropDownMenu->addItem("Plane that least coplanar to any face of the atom", 3);
	_dropDownMenu->addItem("Plane the best fitting to a part of the surface", 4);

	_accepttypes->addItem("NEGATIVE", 0);
	_accepttypes->addItem("POSITIVE", 1);
	_accepttypes->addItem("BOTH", 2);
}
