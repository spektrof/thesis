#pragma once

/*	Keszitette: Lukacs Peter
	
	Felhasznaloi felulet az approximalo programhoz
*/
#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
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
	void RequestWrongCuttingErrorResolve(const char*);
	void SuccessImport();
	void NoAtomLeft(const char*, const bool& = false);
	void InfoShow(const char*);

protected:
	void Init();
	void ResetRequest();
	void AddItemsToDropMenu();
	void SetLabelProperties();
	void SetButtonsProperties();
	void SetDropDownProperties();
	void SetInputLineProperties();
	void CreateHead();
	void CreateStrategiesGroup();
	void CreatePlaneDetails();
	void CreateButtonsGroup();
	void CreateMoreStepsGroup();
	void CreateFourierGroup();
	void CreateBottom();

	private slots:
	void prevAtomEvent();
	void nextAtomEvent();
	void nextPlaneEvent();
	void newprior_event();
	void newcutmode_event();
	void newplane_event();
	void cuttingEvent();
	void undoEvent();
	void acceptEvent();
	void typeaccept_handler();
	void nextNCutEvent();
	void restartEvent();
	void infoEvent();
	void backToMenu();
	void newdisplay();
	void exportEvent();
	void importEvent();

private:
	QWidget *window;

	std::unique_ptr<QVBoxLayout> _mainLayout;

	QLabel* _label, * _info;
	QGroupBox *_strategy, *norms, *coords;

	QLabel *_choice, *_cut;
	QComboBox *_choiceStrategy, *_plane;
	QPushButton *_prev, *_nextChoice, *_nextPlane;

	QLabel * _x, *_y, *_z;
	QLabel * _x2, *_y2, *_z2;
	QLineEdit * _xf, *_yf, *_zf;
	QLineEdit * _xn, *_yn, *_zn;

	QPushButton *_cutting, *_undo, *_accept;
	QComboBox *_acceptTypes;

	QLabel *_nextText;
	QPushButton *_nextNOk;
	QLineEdit *_n;

	QLabel *_displayType;
	QComboBox *_fourierGroups;

	QPushButton* _restart;
	QPushButton* _export;
	QPushButton* _import;
	QPushButton* _moreInfo;
	QPushButton* _back;
	
	std::unique_ptr<QVBoxLayout> head;

	std::unique_ptr<QVBoxLayout> strategiesGroup;
	std::unique_ptr<QHBoxLayout> choiceGroup;
	std::unique_ptr<QHBoxLayout> cutGroup;
	std::unique_ptr<QHBoxLayout> plane_details;

	std::unique_ptr<QVBoxLayout> buttonsGroup;
	std::unique_ptr<QHBoxLayout> acceptGroup;

	std::unique_ptr<QHBoxLayout> moreStepsGorup;
	std::unique_ptr<QHBoxLayout> fourierGorup;

	std::unique_ptr<QHBoxLayout> bottom;

	Request request;
};