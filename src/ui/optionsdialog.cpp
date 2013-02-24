/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "converter/exepath.h"
#include "converter/mediaconverter.h"
#include <QMessageBox>
#include <QSettings>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->spinThreads->setMinimum(1); // at least 1 thread

#ifndef FFMPEG_IN_DATA_PATH
    // initialize program list
    ui->toolTable->verticalHeader()->setVisible(false);
    ui->toolTable->horizontalHeader()->setStretchLastSection(true);
    QList<QString> program_list = ExePath::getPrograms();
    for (int i=0; i<program_list.size(); i++) {
        ui->toolTable->insertRow(i);
        ui->toolTable->setItem(i, 0, new QTableWidgetItem(program_list[i]));
        ui->toolTable->setItem(i, 1, new QTableWidgetItem(""));
        // make tool name not editable
        QTableWidgetItem *item = ui->toolTable->item(i, 0);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    }
#else
    ui->tabTools->setVisible(false);
#endif
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

bool OptionsDialog::exec()
{
    read_fields();
    bool accepted = (QDialog::exec() == QDialog::Accepted);
    if (accepted) {
        write_fields();
    }
    return accepted;
}

bool OptionsDialog::exec_tools()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabTools));
    return exec();
}

void OptionsDialog::read_fields()
{
    QSettings settings;
    ui->spinThreads->setValue(settings.value("options/threads", DEFAULT_THREAD_COUNT).toInt());
    ui->chkHideFormats->setChecked(settings.value("options/hideformats", true).toBool());

#ifndef FFMPEG_IN_DATA_PATH
    // ExePath to table
    const int count = ui->toolTable->rowCount();
    for (int i=0; i<count; i++) {
        QTableWidgetItem *item_program = ui->toolTable->item(i, 0);
        QTableWidgetItem *item_path = ui->toolTable->item(i, 1);
        item_path->setText(ExePath::getPath(item_program->text()));
    }
#endif
}

void OptionsDialog::write_fields()
{
    QSettings settings;
    settings.setValue("options/threads", ui->spinThreads->value());
    settings.setValue("options/hideformats", ui->chkHideFormats->isChecked());

#ifndef FFMPEG_IN_DATA_PATH
    // table to ExePath
    const int count = ui->toolTable->rowCount();
    for (int i=0; i<count; i++) {
        QTableWidgetItem *item_program = ui->toolTable->item(i, 0);
        QTableWidgetItem *item_path = ui->toolTable->item(i, 1);
        ExePath::setPath(item_program->text(), item_path->text());
    }
    // check programs
    QString errmsg;
    if (!MediaConverter::checkExternalPrograms(errmsg)) {
        QMessageBox::critical(this, this->windowTitle(), errmsg);
    }
#endif
}
