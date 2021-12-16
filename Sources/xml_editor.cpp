#include "../Headers/xml_editor.h"
#include "ui_xml_editor.h"
#include "QFileDialog"
#include "QMessageBox"
#include "../Headers/header.h"

XML_Editor::XML_Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::XML_Editor)
{
    ui->setupUi(this);

    for (int i = 0; i < 11; i++){
            tagToChar[open_scheme[i]] = (char)(i+14);
            tagToChar[close_scheme[i]] = (char)(i+33);
    }
    ui->output_textedit->setTabStopDistance(15);
}

XML_Editor::~XML_Editor()
{
    delete ui;
}

void XML_Editor::on_actionOpen_triggered()
{
    //QDir::homePath()
    QString file_name = QFileDialog::getOpenFileName(this, "open XML file", ".", tr("XML & JSON (*.xml *.dxml *.dson)"));
    if(file_name == "") return;
    QString ext = file_name.right(4);
    bool checkable = 1;
    qDebug() << "opening "<< ext << "file";
    ui->statusbar->showMessage("Valid XML");
    if(ext == "dxml" || ext ==  "dson"){
        actionButtons(0);
        if(ext == "dxml"){
            ui->Decomp_xml->setEnabled(true);
            ui->statusbar->showMessage("Compressed XML");
            checkable = 0;
        }
        else if(ext == "dson" && root){
            ui->Decomp_json->setEnabled(true);
            ui->statusbar->showMessage("Compressed JSON");
            checkable = 0;
        }
        else if(!root){
            ui->statusbar->showMessage("Compressed JSON (program can only decompress current session compressed files)");
        }
    }
    else{
        actionButtons(1);
    }
    input_string = open_file(file_name.toStdString());
    try {
        if(checkable) check();
        ui->input_textedit->setTextBackgroundColor(Qt::transparent);
    }  catch (XML_Exception* e) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "XML Invalid",
                              (e->msg + '\n' + "Do you want me to Autocorrect your file? ").c_str()
                              , QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes){
            string output = ident(correct_string(xml));
            ui->output_textedit->setText(QString::fromStdString(output));
            ui->input_textedit->setTextBackgroundColor(Qt::transparent);
            ui->statusbar->showMessage("Autocorrected XML (to complete open valid XML file)");
        }
        else{
            ui->input_textedit->setTextBackgroundColor(Qt::red);
            ui->statusbar->showMessage("Invalid XML");
        }
        actionButtons(0);
    }
    ui->input_textedit->setText(QString::fromStdString(input_string));
    ui->output_textedit->setText("");
}

void XML_Editor::actionButtons(bool b){
    ui->format_button->setEnabled(b);
    ui->convert->setEnabled(b);
    ui->compress_xml->setEnabled(b);
    ui->comp_json->setEnabled(b);
    ui->Decomp_xml->setEnabled(0);
    ui->Decomp_json->setEnabled(0);
}

void XML_Editor::on_format_button_clicked()
{
    if(!INPUT_FILE.size()) XML_Editor::on_actionOpen_triggered();
    string formated_xml = ident(xml);
    ui->output_textedit->setText(QString::fromStdString(formated_xml));
    lastOp = XML;
}

void XML_Editor::on_convert_clicked()
{
    if(!INPUT_FILE.size()) XML_Editor::on_actionOpen_triggered();
    if(!root) root = xml_to_tree(xml);
    string json = tree_to_json(root, normal);
    ui->output_textedit->setText(QString::fromStdString(json));
    lastOp = JSON;
}

void XML_Editor::on_compress_xml_clicked()
{
    if(!INPUT_FILE.size()){
        XML_Editor::on_actionOpen_triggered();
        return;
    }
    string EnXml = encode(xml);
    QString fname = QFileDialog::getSaveFileName(this, "Save Compressed XML", ".", "Compressed XML files (*.dxml)" );
    saveAsFile((fname + ".dxml").toStdString(), EnXml);
}

void XML_Editor::on_comp_json_clicked()
{
    if(!INPUT_FILE.size()){
        XML_Editor::on_actionOpen_triggered();
        return;
    }
    if(!root) root = xml_to_tree(xml);
    string json_comp = tree_to_json(root, compressed);
    QString fname = QFileDialog::getSaveFileName(this, "Save Compressed JSON", ".", "Compressed JSON files (*.dson)" );
    saveAsFile((fname + ".dson").toStdString(), json_comp);
}

void XML_Editor::on_Decomp_xml_clicked()
{
    string xml_converted = decode(input_string);
    ui->output_textedit->setText(QString::fromStdString(ident(xml_converted)));
    lastOp = XML;
}

void XML_Editor::on_Decomp_json_clicked()
{
    string json_converted = decode_json(input_string);
    ui->output_textedit->setText(QString::fromStdString(json_converted));
    lastOp = JSON;
}

void XML_Editor::on_actionSave_as_triggered()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save Output file as", ".", "Allfiles(*)" );
    string output = ui->output_textedit->toPlainText().toStdString();
    if(output.size()){
        if(lastOp == XML)
            saveAsFile((fname + ".xml").toStdString(), output);
        else
            saveAsFile((fname + ".json").toStdString(), output);
    }
}


void XML_Editor::on_actionGithub_triggered()
{
    QMessageBox msg;
    msg.setTextFormat(Qt::RichText);
    msg.setText("<a href=\"https://github.com/MustafaAmer-1/XML-To-JSON\">Github Repository</a>");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

