#include "userinterface.hpp"

UserInterface::UserInterface(QWidget *parent)
	: QWidget(parent)
{
	window = new QWidget();
	_mainLayout = new QVBoxLayout();
	_label = new QLabel("Welcome", this);
	_info = new QLabel("Hey, Im imformation about this ui", this);
	_x = new QLabel("X", this);
	_y = new QLabel("Y", this);
	_z = new QLabel("Z", this);
	_xf = new QLineEdit("0",this);
	_yf = new QLineEdit("0", this);
	_zf = new QLineEdit("26", this);
	_xn = new QLineEdit("0", this);
	_yn = new QLineEdit("0", this);
	_zn = new QLineEdit("1", this);
	_user = new QRadioButton("User", this);
	_autom = new QRadioButton("Automatic", this);
	_cutting = new QPushButton("Cutting",this);
	_undo = new QPushButton("Undo", this);
	_accept = new QPushButton("Accept", this);
	_restart = new QPushButton("Restart", this);
	_moreInfo = new QPushButton("More Info", this);
	_back = new QPushButton("Back", this);
	_automatic_dropdown = new QComboBox();
	_manual_dropdown = new QComboBox();
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
	delete _automatic_dropdown;
	delete _manual_dropdown;
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
	SetInputLineProperties();
	//---------------------------------------
	QVBoxLayout *mainLayout = new QVBoxLayout;

	QVBoxLayout *head = new QVBoxLayout;
	QHBoxLayout *radios = new QHBoxLayout;
	QHBoxLayout *middle_buttons = new QHBoxLayout;
	QHBoxLayout *middle_accept = new QHBoxLayout;
	QVBoxLayout *middle = new QVBoxLayout;
	QVBoxLayout *plane_details = new QVBoxLayout;
	QHBoxLayout *bottom = new QHBoxLayout;
	//------------------------------------------

	CreateHead(head);
	CreateRadios(radios);
	CreateMiddleButtons(middle_buttons);
	CreateMiddle(middle,middle_buttons, middle_accept);
	CreatePlaneDetails(plane_details);
	CreateBottom(bottom);

	//-----------------------------------------	
	mainLayout->addLayout(head);
	mainLayout->addLayout(radios);
	mainLayout->addLayout(middle);
	mainLayout->addLayout(plane_details);
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
	connect(_xf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));
	connect(_yf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));
	connect(_zf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));
	connect(_xn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));
	connect(_yn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));
	connect(_zn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));
	connect(_manual_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(newprior_event()));
	connect(_automatic_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(newprior_event()));

	//-------------------------------------------

	window->setLayout(mainLayout);
	window->show();
}

void UserInterface::radio_handler()
{
		_manual_dropdown->setVisible(_autom->isChecked() ? false : true);
		_automatic_dropdown->setVisible(_autom->isChecked() ? true : false);
		request.IsUserControl =  _autom->isChecked() ? false : true;
}

void UserInterface::cuttingEvent()
{
	request.happen = CUTTING;
	request.ta = TypeAccept(_accepttypes->currentData().toInt());
	request.uc = _user->isChecked() ? UserControl(_manual_dropdown->currentData().toInt()) : UserControl(_automatic_dropdown->currentData().toInt());

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
	_automatic_dropdown->setVisible(false);
	_manual_dropdown->setVisible(false);
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

	_manual_dropdown->setVisible(false);
	_automatic_dropdown->setVisible(true);

	_user->setChecked(false);
	_autom->setChecked(true);

	_moreInfo->setVisible(true);
}

void UserInterface::typeaccept_handler()
{
	request.ta = TypeAccept(_accepttypes->currentData().toInt());
}

void UserInterface::newplane_event()
{
	request.happen = NEWPLANE;

	request.plane_coord = Coord(_xf->text().toFloat(), _yf->text().toFloat(), _zf->text().toFloat());
	request.plane_norm = Coord(_xn->text().toFloat(), _yn->text().toFloat(), _zn->text().toFloat());
}

void UserInterface::newprior_event()
{
	request.happen = NEWSTRATEGY;
	if (request.IsUserControl)
	{
		request.uc = UserControl(_manual_dropdown->currentData().toInt());
	}
	else
	{
		request.ac = AutomaticControl(_automatic_dropdown->currentData().toInt());
	}
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
void UserInterface::SetButtonsProperties() 
{
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
	_manual_dropdown->setStyleSheet("background-color: #bcee68;");
	_automatic_dropdown->setStyleSheet("background-color: #bcee68;");
	_accepttypes->setStyleSheet("background-color: #bcee68;");
}
void UserInterface::SetDropDownProperties()
{
	_manual_dropdown->setFixedHeight(25);
	_automatic_dropdown->setFixedHeight(25);
	_accepttypes->setFixedHeight(25);

	_automatic_dropdown->setEditable(true);
	_manual_dropdown->setEditable(true);
	_accepttypes->setEditable(true);

	_accepttypes->setEnabled(false);

	_automatic_dropdown->lineEdit()->setReadOnly(true);
	_manual_dropdown->lineEdit()->setReadOnly(true);
	_accepttypes->lineEdit()->setReadOnly(true);

	_manual_dropdown->lineEdit()->setAlignment(Qt::AlignCenter);
	_automatic_dropdown->lineEdit()->setAlignment(Qt::AlignCenter);
	_accepttypes->lineEdit()->setAlignment(Qt::AlignCenter);

	for (int i = 0; i < _manual_dropdown->count(); ++i)
	{
		_manual_dropdown->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _automatic_dropdown->count(); ++i)
	{
		_automatic_dropdown->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _accepttypes->count(); ++i)
	{
		_accepttypes->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
}
void UserInterface::SetInputLineProperties()
{
	_xf->setFixedWidth(25);
	_x->setFixedWidth(15);
	_yf->setFixedWidth(25);
	_y->setFixedWidth(15);
	_zf->setFixedWidth(25);
	_z->setFixedWidth(15);
	_xn->setFixedWidth(25);
	_yn->setFixedWidth(25);
	_zn->setFixedWidth(25);

	_xf->setStyleSheet("background-color: #bcee68;");
	_yf->setStyleSheet("background-color: #bcee68;");
	_zf->setStyleSheet("background-color: #bcee68;");
	_xn->setStyleSheet("background-color: #bcee68;");
	_yn->setStyleSheet("background-color: #bcee68;");
	_zn->setStyleSheet("background-color: #bcee68;");

	_xn->setAlignment(Qt::AlignCenter);
	_yn->setAlignment(Qt::AlignCenter);
	_zn->setAlignment(Qt::AlignCenter);
	_xf->setAlignment(Qt::AlignCenter);
	_yf->setAlignment(Qt::AlignCenter);
	_zf->setAlignment(Qt::AlignCenter);

//	_xn->setInputMask("0.00");
	//_xn->setMaxLength(4);

	//_xn->setValidator(new QDoubleValidator(0.0, 100.0, 1));
	//QFloatValidator *v = new QIntValidator(0, 100);
//	e.setValidator(v);

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
	_manual_dropdown->setVisible(_autom->isChecked() ? false : true);
	_automatic_dropdown->setVisible(_autom->isChecked() ? true : false);

	SetDropDownProperties();

	middle_accept->addWidget(_accepttypes);
	middle_accept->addWidget(_accept);

	middle->addWidget(_automatic_dropdown);
	middle->addWidget(_manual_dropdown);
	middle->addLayout(middle_buttons);
	middle->addLayout(middle_accept);

}
void UserInterface::CreatePlaneDetails(QVBoxLayout* plane_details)
{

	QHBoxLayout * _xline = new QHBoxLayout();
	QHBoxLayout * _yline = new QHBoxLayout();
	QHBoxLayout * _zline = new QHBoxLayout();

	_xline->addWidget(_x);
	_yline->addWidget(_y);
	_zline->addWidget(_z);

	_xline->addWidget(_xf, 0, Qt::AlignLeft);
	_yline->addWidget(_yf, 0, Qt::AlignLeft);
	_zline->addWidget(_zf, 0, Qt::AlignLeft);

	_xline->addWidget(_xn, 0, Qt::AlignLeft);
	_yline->addWidget(_yn, 0, Qt::AlignLeft);
	_zline->addWidget(_zn, 0, Qt::AlignLeft);

	plane_details->addLayout(_xline);
	plane_details->addLayout(_yline);
	plane_details->addLayout(_zline);
}
void UserInterface::CreateBottom(QHBoxLayout* bottom)
{
	bottom->addWidget(_restart, 0, Qt::AlignBottom | Qt::AlignLeft);
	bottom->addWidget(_moreInfo, 0, Qt::AlignBottom | Qt::AlignRight);
	bottom->addWidget(_back, 0,  Qt::AlignRight);
}

void UserInterface::AddItemsToDropMenu()
{
	_accepttypes->addItem("NEGATIVE", 0);
	_accepttypes->addItem("POSITIVE", 1);
	_accepttypes->addItem("BOTH", 2);
	
	
	_automatic_dropdown->addItem("Legnagyobb terfogatu", 0);
	_automatic_dropdown->addItem("Legnagyobb atmeroju", 1);
	_automatic_dropdown->addItem("Legregebb ideje erintetlen", 2);
	_automatic_dropdown->addItem("Optimalis(parameteres)", 3);
	_automatic_dropdown->addItem("Optimalis + atmero", 4);
	_automatic_dropdown->addItem("Optimalis + terfogat", 5);
	_automatic_dropdown->addItem("Manualis", 6);

	
	//	Vágó sík :

	_manual_dropdown->addItem("Manualis", 0);
	_manual_dropdown->addItem("Veletlen normalisu, sulyponton atmeno", 1);
	_manual_dropdown->addItem("Atmerore meroleges, sulyponton atmeno", 2);
	_manual_dropdown->addItem("Veletlen lap alatt fekvo", 3);
	_manual_dropdown->addItem("Optimalis lap alatt fekvo", 4);
	_manual_dropdown->addItem("Osszes pontra illesztett", 5);
	_manual_dropdown->addItem("Veletlen feluletre illesztett", 6);
	_manual_dropdown->addItem("Optimalis feluletre illesztett", 7);
	_manual_dropdown->addItem("Globalis hibara optimalis(? )", 8);
}

void UserInterface::RequestWrongCuttingErrorResolve()
{
	QMessageBox::warning(this, tr("WRONG CUT"),tr("U ARE STUPID!"), QMessageBox::Ok | QMessageBox::Help);

	_accept->setEnabled(false);
	_accepttypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);
}

void UserInterface::ErrorShow(const char* text)
{
	QMessageBox::information(this, tr("ERROR"), tr(text), QMessageBox::Ok | QMessageBox::Help);
}