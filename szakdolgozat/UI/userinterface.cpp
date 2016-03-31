#include "userinterface.hpp"

UserInterface::UserInterface(QWidget *parent)
	: QWidget(parent)
{
	window = new QWidget();
	_mainLayout = new QVBoxLayout();
	_label = new QLabel("Welcome", this);
	_info = new QLabel("Hey, Im imformation about this ui", this);

	_choice = new QLabel("Choice", this);
	_cut = new QLabel("Cut", this);

	_strategy = new QGroupBox(tr("Strategies"),this);
	norms = new QGroupBox(tr("Normals"));
	coords = new QGroupBox(tr("Coords"));

	_choiceStrategy = new QComboBox();
	_plane = new QComboBox();

	_prev = new QPushButton("Prev", this);
	_nextChoice = new QPushButton("Next", this);
	_nextPlane = new QPushButton("Next", this);

	_x = new QLabel("X", this);	_x2 = new QLabel("X", this);
	_y = new QLabel("Y", this); _y2 = new QLabel("Y", this);
	_z = new QLabel("Z", this); _z2 = new QLabel("Z", this);
	_xf = new QLineEdit("0", this);
	_yf = new QLineEdit("0", this);
	_zf = new QLineEdit("26", this);
	_xn = new QLineEdit("0", this);
	_yn = new QLineEdit("0", this);
	_zn = new QLineEdit("1", this);

	_cutting = new QPushButton("Cutting", this);
	_undo = new QPushButton("Undo", this);
	_accept = new QPushButton("Accept", this);

	_acceptTypes = new QComboBox();

	_nextText = new QLabel("Next n steps", this);
	_nextNOk = new QPushButton("OK", this);
	_n = new QLineEdit("1",this);

	_displayType = new QLabel("Megj. atomok fourier egyutthato alapjan:",this);
	_fourierGroups = new QComboBox();

	_restart = new QPushButton("Restart", this);
	_moreInfo = new QPushButton("More Info", this);
	_back = new QPushButton("Back", this);

	AddItemsToDropMenu();

	head = new QVBoxLayout;
	strategiesGroup = new QVBoxLayout;
	choiceGroup = new QHBoxLayout;
	cutGroup = new QHBoxLayout;
	buttonsGroup = new QVBoxLayout;
    plane_details = new QHBoxLayout;
	acceptGroup = new QHBoxLayout;
	moreStepsGorup = new QHBoxLayout;
	fourierGorup = new QHBoxLayout;
	bottom = new QHBoxLayout;

	Init();
}

UserInterface::~UserInterface()
{
	delete _label;
	delete _info;

	delete _choice;
	delete _cut;
	delete _choiceStrategy;
	delete _plane;
	delete _prev;
	delete _nextChoice;
	delete _nextPlane;

	delete _x;
	delete _y;
	delete _z;
	delete _x2;
	delete _y2;
	delete _z2;
	delete _xf;
	delete _yf;
	delete _zf;
	delete _xn;
	delete _yn;
	delete _zn;
	delete norms;
	delete coords;
	delete _strategy;

	delete _cutting;
	delete _undo;
	delete _accept;
	delete _acceptTypes;

	delete _nextText;
	delete _nextNOk;
	delete _n;

	delete _restart;
	delete _moreInfo;
	delete _back;

	delete window;
	delete _mainLayout;
}

void UserInterface::Init() 
{
	window->setStyleSheet("background-color: #7ac5cd;");
	window->setFixedSize(400, 400);
	window->move(900, 200);
	//---------------------------------------
	SetLabelProperties();
	SetButtonsProperties();
	SetInputLineProperties();
	//---------------------------------------
	QVBoxLayout *mainLayout = new QVBoxLayout;

	//------------------------------------------

	CreateHead();
	CreateStrategiesGroup();
	CreatePlaneDetails();
	CreateButtonsGroup();
	CreateMoreStepsGroup();
	CreateFourierGroup();
	CreateBottom();

	//-----------------------------------------	
	mainLayout->addLayout(head);
	mainLayout->addWidget(_strategy);
	mainLayout->addLayout(buttonsGroup);
	mainLayout->addLayout(moreStepsGorup);
	mainLayout->addLayout(fourierGorup);
	mainLayout->addLayout(bottom);

	//_----------------------------------------- connects

	connect(_prev, SIGNAL(clicked()), this, SLOT(prevAtomEvent()));
	connect(_nextChoice, SIGNAL(clicked()), this, SLOT(nextAtomEvent()));
	connect(_nextPlane, SIGNAL(clicked()), this, SLOT(nextPlaneEvent()));
	connect(_choiceStrategy, SIGNAL(currentIndexChanged(int)), this, SLOT(newprior_event()));
	connect(_plane, SIGNAL(currentIndexChanged(int)), this, SLOT(newcutmode_event()));	//nem küld eventet

	connect(_xf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));	//küld eventet, a textedited nem küld
	connect(_yf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));
	connect(_zf, SIGNAL(textChanged(const QString)), this, SLOT(newplane_event()));
	connect(_xn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));
	connect(_yn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));
	connect(_zn, SIGNAL(editingFinished()), this, SLOT(newplane_event()));

	connect(_cutting, SIGNAL(clicked()), this, SLOT(cuttingEvent()));
	connect(_undo, SIGNAL(clicked()), this, SLOT(undoEvent()));
	connect(_accept, SIGNAL(clicked()), this, SLOT(acceptEvent()));
	connect(_acceptTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(typeaccept_handler()));

	connect(_nextNOk, SIGNAL(clicked()), this, SLOT(nextNCutEvent()));
	connect(_fourierGroups, SIGNAL(currentIndexChanged(int)), this, SLOT(newdisplay()));

	connect(_restart, SIGNAL(clicked()), this, SLOT(restartEvent()));
	connect(_moreInfo, SIGNAL(clicked()), this, SLOT(infoEvent()));
	connect(_back, SIGNAL(clicked()), this, SLOT(backToMenu()));


	//-------------------------------------------

	window->setLayout(mainLayout);
	window->show();
}

void UserInterface::cuttingEvent()
{
	request.eventtype = CUTTING;
	
	_undo->setEnabled(request.cut_mode == MANUAL ? true : false);
	_acceptTypes->setEnabled(request.cut_mode == MANUAL ? true : false);
	_accept->setEnabled(request.cut_mode == MANUAL ? true : false);
	_cutting->setEnabled(request.cut_mode == MANUAL ? false : true);

	_choiceStrategy->setEnabled(request.cut_mode == MANUAL ? false : true);
	_plane->setEnabled(request.cut_mode == MANUAL ? false : true);
	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false);
	_nextChoice->setEnabled(request.cut_mode == MANUAL ? false : true);
	_prev->setEnabled(request.cut_mode == MANUAL ? false : true);
}

void UserInterface::undoEvent()
{
	request.eventtype = UNDO;

	_accept->setEnabled(false);
	_acceptTypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);

	_choiceStrategy->setEnabled(true);
	_plane->setEnabled(true);
	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false); //HA RANDOM
	_nextChoice->setEnabled(true);
	_prev->setEnabled(true);
}

void UserInterface::acceptEvent()
{
	request.eventtype = ACCEPT;
	request.type = TypeOfAccept(_acceptTypes->currentData().toInt());

	_accept->setEnabled(false);
	_acceptTypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);

	_choiceStrategy->setEnabled(true);
	_plane->setEnabled(true);
	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false); //HA RANDOM
	_nextChoice->setEnabled(true);
	_prev->setEnabled(true);
}

void UserInterface::restartEvent()
{
	request = Request(RESTART, ChoiceMode(_choiceStrategy->currentData().toInt()));

	_n->setText("1"); 
	_xn->setText("0"); _yn->setText("0"); _zn->setText("1");

	_choiceStrategy->setEnabled(true);
	_prev->setEnabled(true);
	_nextChoice->setEnabled(true);
	_plane->setEnabled(true);

	_accept->setEnabled(false);
	_acceptTypes->setEnabled(false);
	_undo->setEnabled(false);
	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false); //HA RANDOM
	_cutting->setEnabled(true);

	_nextNOk->setEnabled(false);
	_n->setEnabled(false);
}

void UserInterface::typeaccept_handler()
{
	request.type = TypeOfAccept(_acceptTypes->currentData().toInt());
}

void UserInterface::newplane_event()
{
	request.eventtype = NEWPLANE;

	request.plane_coord = Coord(_xf->text().toFloat(), _yf->text().toFloat(), _zf->text().toFloat());
	request.plane_norm = Coord(_xn->text().toFloat(), _yn->text().toFloat(), _zn->text().toFloat());
}
void UserInterface::newprior_event()
{
	request.eventtype = NEWSTRATEGY;
	request.choice = ChoiceMode(_choiceStrategy->currentData().toInt());


}
void UserInterface::newcutmode_event()
{
	request.eventtype = NEWCUTTINGMODE;
	request.cut_mode = CuttingMode(_plane->currentData().toInt());

	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false); //HA RANDOM
	coords->setVisible(request.cut_mode == MANUAL ? true : false);
	norms->setVisible(request.cut_mode == MANUAL ? true : false);

	_nextNOk->setEnabled(request.cut_mode == MANUAL ?  false : true);
	_n->setEnabled(request.cut_mode == MANUAL ?  false : true);
}
void UserInterface::newdisplay()
{
	request.eventtype = NEWDISPLAY;

	request.disp = Display(_fourierGroups->currentData().toInt());
}

void UserInterface::prevAtomEvent()
{
	request.eventtype = PREVATOM;
}
void UserInterface::nextAtomEvent()
{
	request.eventtype = NEXTATOM;
}
void UserInterface::nextPlaneEvent()
{
	request.eventtype = RECALCULATING;
}
void UserInterface::nextNCutEvent()
{
	request.eventtype = MORESTEPS;
	request.CountsOfCutting = _n->text().toInt();
}

/*TODO : aktualizalni*/
void UserInterface::infoEvent()
{
	_info->setVisible(true);
	_back->setVisible(true);

	_label->setVisible(false);
	_cutting->setVisible(false);
	_undo->setVisible(false);
	_accept->setVisible(false);
	_restart->setVisible(false);
	_acceptTypes->setVisible(false);

	_strategy->setVisible(false);
	_nextNOk->setVisible(false);
	_nextText->setVisible(false);
	_n->setVisible(false);

	_moreInfo->setVisible(false);


}
void UserInterface::backToMenu()
{
	_info->setVisible(false);
	_back->setVisible(false);

	_label->setVisible(true);

	_cutting->setVisible(true);
	_undo->setVisible(true);
	_accept->setVisible(true);
	_restart->setVisible(true);
	_acceptTypes->setVisible(true);

	_strategy->setVisible(true);
	_nextNOk->setVisible(true);
	_nextText->setVisible(true);
	_n->setVisible(true);

	_moreInfo->setVisible(true);
}


Request UserInterface::GetRequest() 
{
	if (request.eventtype == NONE) return request;
	
	Request result(request); 

	ResetRequest();
	if (result.eventtype == ACCEPT)
	{
		request.type = NEGATIVE;
		_acceptTypes->setCurrentIndex(0);
	
	}
	else if (result.eventtype == RESTART)	//restarttal osszefuggo valtozasok
	{
		//_choiceStrategy->setCurrentIndex(0);
		_plane->setCurrentIndex(0);
		_acceptTypes->setCurrentIndex(0);
		_xf->setText("0"); _yf->setText("0"); _zf->setText("26");
	}
	return result;
}

void UserInterface::ResetRequest() 
{
	request.eventtype = NONE;
}

void UserInterface::SetLabelProperties()
{
	_label->setFont(QFont("Courier New", 16, QFont::Bold));
	//	label->setAlignment(Qt::AlignHCenter);
	_label->setMaximumHeight(25);

	_choice->setFont(QFont("Courier New", 10, QFont::Bold));
	_choice->setMaximumHeight(25);

	_cut->setFont(QFont("Courier New", 10, QFont::Bold));
	_cut->setMaximumHeight(25);

	_displayType->setFont(QFont("Courier New", 10, QFont::Bold));
	_displayType->setMaximumHeight(25);

	_nextText->setFont(QFont("Courier New", 10, QFont::Bold));
	_nextText->setAlignment(Qt::AlignCenter);
	_nextText->setMaximumHeight(25);
	_nextText->setStyleSheet("margin-left : 100px;");

}
void UserInterface::SetButtonsProperties() 
{
	_cutting->setFixedSize(80, 30);
	_undo->setFixedSize(80, 30);
	_accept->setFixedSize(80, 30);

	_nextChoice->setFixedSize(50, 25);
	_nextNOk->setFixedSize(50, 25);
	_nextPlane->setFixedSize(50, 25);
	_prev->setFixedSize(50, 25);

	_accept->setEnabled(false);
	_undo->setEnabled(false);
	_nextPlane->setEnabled(request.cut_mode > 5 ? true : false); //HA RANDOM

	_back->setVisible(false);

	_cutting->setStyleSheet("background-color: #bcee68;");
	_undo->setStyleSheet("background-color: #bcee68;");
	_accept->setStyleSheet("background-color: #bcee68;");
	_restart->setStyleSheet("background-color: #bcee68;");
	_moreInfo->setStyleSheet("background-color: #bcee68;");
	_back->setStyleSheet("background-color: #bcee68;");
	_nextChoice->setStyleSheet("background-color: #bcee68;");
	_nextNOk->setStyleSheet("background-color: #bcee68;");
	_nextPlane->setStyleSheet("background-color: #bcee68;");
	_prev->setStyleSheet("background-color: #bcee68;");
	_plane->setStyleSheet("background-color: #bcee68;");
	_choiceStrategy->setStyleSheet("background-color: #bcee68;");
	_acceptTypes->setStyleSheet("background-color: #bcee68;");
	_fourierGroups->setStyleSheet("background-color: #bcee68;");
}
void UserInterface::SetDropDownProperties()
{
	_choiceStrategy->setFixedSize(150, 20);
	_plane->setFixedSize(220,20);
	_acceptTypes->setFixedSize(80, 30);
	_fourierGroups->setFixedSize(120, 20);

	_choiceStrategy->setEditable(true);
	_plane->setEditable(true);
	_acceptTypes->setEditable(true);
	_fourierGroups->setEditable(true);

	_acceptTypes->setEnabled(false);

	_choiceStrategy->lineEdit()->setReadOnly(true);
	_plane->lineEdit()->setReadOnly(true);
	_acceptTypes->lineEdit()->setReadOnly(true);
	_fourierGroups->lineEdit()->setReadOnly(true);

	_plane->lineEdit()->setAlignment(Qt::AlignCenter);
	_choiceStrategy->lineEdit()->setAlignment(Qt::AlignCenter);
	_acceptTypes->lineEdit()->setAlignment(Qt::AlignCenter);
	_fourierGroups->lineEdit()->setAlignment(Qt::AlignCenter);

	for (int i = 0; i < _plane->count(); ++i)
	{
		_plane->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _choiceStrategy->count(); ++i)
	{
		_choiceStrategy->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _acceptTypes->count(); ++i)
	{
		_acceptTypes->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	for (int i = 0; i < _fourierGroups->count(); ++i)
	{
		_fourierGroups->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
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

	_n->setFixedWidth(25);

	_xf->setStyleSheet("background-color: #bcee68;");
	_yf->setStyleSheet("background-color: #bcee68;");
	_zf->setStyleSheet("background-color: #bcee68;");
	_xn->setStyleSheet("background-color: #bcee68;");
	_yn->setStyleSheet("background-color: #bcee68;");
	_zn->setStyleSheet("background-color: #bcee68;");
	_n->setStyleSheet("background-color: #bcee68;");

	_xn->setAlignment(Qt::AlignCenter);
	_yn->setAlignment(Qt::AlignCenter);
	_zn->setAlignment(Qt::AlignCenter);
	_xf->setAlignment(Qt::AlignCenter);
	_yf->setAlignment(Qt::AlignCenter);
	_zf->setAlignment(Qt::AlignCenter);
	_n->setAlignment(Qt::AlignCenter);


	request.plane_coord = Coord(_xf->text().toFloat(), _yf->text().toFloat(), _zf->text().toFloat());
	request.plane_norm = Coord(_xn->text().toFloat(), _yn->text().toFloat(), _zn->text().toFloat());
}

void UserInterface::CreateHead()
{
	head->addWidget(_label, 0, Qt::AlignHCenter);
	head->addWidget(_info, 0, Qt::AlignHCenter);

	head->setSpacing(0);
	head->setMargin(0);

	_info->setVisible(false);
}
void UserInterface::CreateButtonsGroup()
{
	QHBoxLayout *upperButtons = new QHBoxLayout();

	upperButtons->addWidget(_cutting, 0, Qt::AlignBottom );
	upperButtons->addWidget(_undo, 0, Qt::AlignBottom);
	
	acceptGroup->addWidget(_acceptTypes, 0, Qt::AlignBottom);
	acceptGroup->addWidget(_accept, 0, Qt::AlignBottom);

	buttonsGroup->addLayout(upperButtons);
	buttonsGroup->addLayout(acceptGroup);
	//buttonsGroup->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

}

void UserInterface::CreateStrategiesGroup()
{
	SetDropDownProperties();

	choiceGroup->addWidget(_choice, 0, Qt::AlignLeft);
	choiceGroup->addWidget(_choiceStrategy, 0, Qt::AlignLeft);
	choiceGroup->addWidget(_prev, 0, Qt::AlignLeft);
	choiceGroup->addWidget(_nextChoice, 0, Qt::AlignLeft);

	cutGroup->addWidget(_cut, 0, Qt::AlignLeft);
	cutGroup->addWidget(_plane, 0, Qt::AlignLeft);
	cutGroup->addWidget(_nextPlane, 0, Qt::AlignLeft);

	strategiesGroup->addLayout(choiceGroup);
	strategiesGroup->addLayout(cutGroup);

	//strategiesGroup->setAlignment(choiceGroup, Qt::AlignTop);
	//strategiesGroup->setAlignment(cutGroup, Qt::AlignTop);

	_strategy->setLayout(strategiesGroup);
}

void UserInterface::CreateMoreStepsGroup()
{
	moreStepsGorup->addWidget(_nextText);
	moreStepsGorup->addWidget(_n);
	moreStepsGorup->addWidget(_nextNOk);

	moreStepsGorup->setAlignment(Qt::AlignTrailing | Qt::AlignTrailing);
	moreStepsGorup->addStretch(1);

	_nextNOk->setEnabled(request.cut_mode == MANUAL ? false : true);
	_n->setEnabled(request.cut_mode == MANUAL ? false : true);
}

void UserInterface::CreateFourierGroup()
{
	fourierGorup->addWidget(_displayType);
	fourierGorup->addWidget(_fourierGroups);

	fourierGorup->setAlignment(Qt::AlignTrailing | Qt::AlignTrailing);
	fourierGorup->addStretch(1);
}

void UserInterface::CreatePlaneDetails()
{
	QHBoxLayout *planegroup = new QHBoxLayout();
	QHBoxLayout *left = new QHBoxLayout();
	QHBoxLayout *right = new QHBoxLayout();

	QVBoxLayout *coordnames = new QVBoxLayout();
	QVBoxLayout *coordnames2 = new QVBoxLayout();
	QVBoxLayout *coorddata = new QVBoxLayout();
	QVBoxLayout *normaldata = new QVBoxLayout();

	coordnames->addWidget(_x, 0, Qt::AlignCenter);
	coordnames->addWidget(_y, 0, Qt::AlignCenter);
	coordnames->addWidget(_z, 0, Qt::AlignCenter);

	coordnames2->addWidget(_x2, 0, Qt::AlignCenter);
	coordnames2->addWidget(_y2, 0, Qt::AlignCenter);
	coordnames2->addWidget(_z2, 0, Qt::AlignCenter);

	coorddata->addWidget(_xf, 0, Qt::AlignCenter);
	coorddata->addWidget(_yf, 0, Qt::AlignCenter);
	coorddata->addWidget(_zf, 0, Qt::AlignCenter);

	normaldata->addWidget(_xn, 0, Qt::AlignCenter);
	normaldata->addWidget(_yn, 0, Qt::AlignCenter);
	normaldata->addWidget(_zn, 0, Qt::AlignCenter);

	left->addLayout(coordnames);
	left->addLayout(coorddata);
	coords->setLayout(left);
	coords->setFixedSize(125, 100);

	right->addLayout(coordnames2);
	right->addLayout(normaldata);
	norms->setLayout(right);
	norms->setFixedSize(125, 100);

	plane_details->addWidget(coords);
	plane_details->addWidget(norms);

	coords->setVisible(request.cut_mode == MANUAL ? true : false);
	norms->setVisible(request.cut_mode == MANUAL ? true : false);

	strategiesGroup->addLayout(plane_details);

}
void UserInterface::CreateBottom()
{
	bottom->addWidget(_restart, 0, Qt::AlignBottom | Qt::AlignLeft);
	bottom->addWidget(_moreInfo, 0, Qt::AlignBottom | Qt::AlignRight);
	bottom->addWidget(_back, 0, Qt::AlignRight);
}

void UserInterface::AddItemsToDropMenu()
{
	_acceptTypes->addItem("NEGATIVE", 0);
	_acceptTypes->addItem("POSITIVE", 1);
	_acceptTypes->addItem("BOTH", 2);
	
	_choiceStrategy->addItem("Legnagyobb terfogatu", 0);
	_choiceStrategy->addItem("Legnagyobb atmeroju", 1);
	_choiceStrategy->addItem("Legregebb ideje erintetlen", 2);
	_choiceStrategy->addItem("Optimalis(parameteres)", 3);
	_choiceStrategy->addItem("Optimalis + atmero", 4);
	_choiceStrategy->addItem("Optimalis + terfogat", 5);

	//	Vágó sík :

	_plane->addItem("Manualis", 0);
	_plane->addItem("Optimalis lap alatt fekvo", 1);
	_plane->addItem("Osszes pontra illesztett", 2);
	_plane->addItem("Optimalis feluletre illesztett", 3);
	_plane->addItem("Globalis hibara optimalis(? )", 4);
	_plane->addItem("Atmerovel megegyezo iranyu, sulyponton atmeno", 5);
	_plane->addItem("Veletlen", 6);
	_plane->addItem("Veletlen normalisu, sulyponton atmeno", 7);
	_plane->addItem("Veletlen feluletre illesztett", 8);
	_plane->addItem("Veletlen lap alatt fekvo", 9);

	_fourierGroups->addItem("Elo atomok", 0);
	_fourierGroups->addItem("Relevans atomok", 1);
}

void UserInterface::RequestWrongCuttingErrorResolve()
{
	QMessageBox::warning(this, tr("WRONG CUT"),tr("U ARE STUPID!"), QMessageBox::Ok | QMessageBox::Help);

	_accept->setEnabled(false);
	_acceptTypes->setEnabled(false);
	_undo->setEnabled(false);
	_cutting->setEnabled(true);
}

void UserInterface::ErrorShow(const char* text)
{
	QMessageBox::information(this, tr("ERROR"), tr(text), QMessageBox::Ok | QMessageBox::Help);
}