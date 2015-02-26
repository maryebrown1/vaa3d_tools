#ifndef NEURON_SEPERATOR_EXPLORER_H
#define NEURON_SEPERATOR_EXPLORER_H

#include <QDialog>
#include <QtGui>
#include <v3d_interface.h>

#define NAME_INWIN "Neuron_Seperator_Result_Explorer_OrginalImage"
#define NAME_OUTWIN "Neuron_Seperator_Result_Explorer_SeperateNeuron"

typedef struct{
    int status; //0: not checked; 1: checked; 2: outlier
    QList<QString> fnames_extract;
    QList<int> status_extract; //0: not checked; 1: accept; 2: reject
}extraction;

class neuron_seperator_explorer : public QDialog
{
    Q_OBJECT
public:
    neuron_seperator_explorer(V3DPluginCallback2 * cb, QWidget *parent = 0);
    ~neuron_seperator_explorer() {}

signals:

public slots:
    void acceptExt();
    void rejectExt();
    void rejectImg();
    void preExt();
    void preImg();
    void nextImg();
    void save();
    void loadPoj();

private:
    void creat();
    void checkButton();
    void updateAll();
    void updateOutputWindow();
    void updateInputWindow();

private:
    V3DPluginCallback2 * callback;
    QString anopath;
    QList<QString> fnames_ano;
    QList<extraction> imgs;
    int idx_img, idx_ext, pre_img, pre_ext;
    unsigned char *image1Dc_in;
    unsigned char *image1Dc_out;
    V3DLONG sz_img[4];
    V3DLONG sz_out[4];
    int intype, outtype;

public:
    QLineEdit *edit_dir,*edit_curimg,*edit_curext;
    QPushButton *btn_acceptExt, *btn_rejectExt, *btn_rejectImg;
    QPushButton *btn_preExt, *btn_preImg, *btn_nextImg;
    QPushButton *btn_save, *btn_quit;
    QPushButton *btn_loadDir, *btn_loadPoj;
};

#endif // NEURON_SEPERATOR_EXPLORER_H