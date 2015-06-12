#include "manual_correct_dialog.h"
#define main_win_name "spine_detector_result"
#define fname_image "truncated_view"

manual_correct_dialog::manual_correct_dialog(V3DPluginCallback2 *cb)
{
    callback=cb;
    image1Dc_in=0;
    image1Dc_spine=0;
    create();
    image_trun=0;
    edit_flag=false;
}

void manual_correct_dialog::create()
{
    QGridLayout *mygridLayout = new QGridLayout;
    QLabel* label_load = new QLabel(QObject::tr("Load Image:"));
    mygridLayout->addWidget(label_load,0,0,1,1);
    edit_load = new QLineEdit;
    edit_load->setText(""); edit_load->setReadOnly(true);
    mygridLayout->addWidget(edit_load,0,1,1,6);
    btn_load = new QPushButton("...");
    mygridLayout->addWidget(btn_load,0,7,1,1);

    QLabel* label_swc = new QLabel(QObject::tr("Load swc:"));
    mygridLayout->addWidget(label_swc,1,0,1,1);
    edit_swc = new QLineEdit;
    edit_swc->setText(""); edit_swc->setReadOnly(true);
    mygridLayout->addWidget(edit_swc,1,1,1,6);
    btn_swc = new QPushButton("...");
    mygridLayout->addWidget(btn_swc,1,7,1,1);

    QLabel* label_csv = new QLabel(QObject::tr("Output directory:"));
    mygridLayout->addWidget(label_csv,2,0,1,1);
    edit_csv = new QLineEdit;
    edit_csv->setText(""); edit_csv->setReadOnly(true);
    mygridLayout->addWidget(edit_csv,2,1,1,6);
    btn_csv = new QPushButton("...");
    mygridLayout->addWidget(btn_csv,2,7,1,1);

    QLabel *channel = new QLabel(tr("Which channel to use?"));
    channel_menu = new QComboBox;
    channel_menu->addItem("red");
    channel_menu->addItem("green");
    channel_menu->addItem("blue");
    mygridLayout->addWidget(channel,3,0,1,2);
    mygridLayout->addWidget(channel_menu,3,3,1,5);

    //para setting
    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    line_1->setFrameShadow(QFrame::Sunken);
    mygridLayout->addWidget(line_1,4,0,1,8);

    QLabel *bg_thr = new QLabel(tr("Background threshold:"));
    mygridLayout->addWidget(bg_thr,5,0,1,6);
    QLabel *max_pixel=new QLabel (tr("Max spine volume:"));
    mygridLayout->addWidget(max_pixel,6,0,1,6);
    QLabel *min_pixel=new QLabel (tr("Min spine volume:"));
    mygridLayout->addWidget(min_pixel,7,0,1,6);
    QLabel *max_dis=new QLabel(tr("Max spine distance to surface:"));
    mygridLayout->addWidget(max_dis,8,0,1,6);
    QLabel *width_thr=new QLabel(tr("Max spine width:"));
    mygridLayout->addWidget(width_thr,9,0,1,6);

    spin_max_dis=new QSpinBox;
    spin_max_dis->setRange(5,80);
    spin_max_dis->setValue(40);
    spin_min_pixel=new QSpinBox;
    spin_min_pixel->setRange(10,100);
    spin_min_pixel->setValue(30);
    spin_bg_thr=new QSpinBox;
    spin_bg_thr->setRange(1,255);
    spin_bg_thr->setValue(90);
    spin_max_pixel=new QSpinBox;
    spin_max_pixel->setRange(2000,8000);
    spin_max_pixel->setValue(7000);
    spin_width_thr=new QSpinBox;
    spin_width_thr->setRange(10,100);
    spin_width_thr->setValue(35);
    mygridLayout->addWidget(spin_bg_thr,5,6,1,2);
    mygridLayout->addWidget(spin_max_pixel,6,6,1,2);
    mygridLayout->addWidget(spin_min_pixel,7,6,1,2);
    mygridLayout->addWidget(spin_max_dis,8,6,1,2);
    mygridLayout->addWidget(spin_width_thr,9,6,1,2);

    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);
    mygridLayout->addWidget(line_2,15,0,1,8);
    QPushButton *ok     = new QPushButton("OK");
    QPushButton *cancel = new QPushButton("Cancel");
    mygridLayout->addWidget(ok,16,2,1,2);
    mygridLayout->addWidget(cancel,16,5,1,2);

    //operation zone
    QFrame *line_3 = new QFrame();
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);
    mygridLayout->addWidget(line_3,17,0,1,8);
    QString info=">> Spine detector first perform automatic spines detections near dendrites,then allows"
            " users to do manual proofreading.<br>";
    info+=">> <b>Background threshold: </b> All voxels above the background threshold are foreground, others are background voxles (voxels not considered spines).<br>";
    info+=">> <b>Max spine volume:</b> Maximum number of voxels allowed for a spine. Groups with larger volumes are considered not spines.<br>";
    info+=">> <b>Min spine volume:</b> Minumum number of voxels allowed for a spine.<br>";
    info+=">> <b>Max spine distance:</b> Spine distance is defined as the distance from the surface of dendrite. Automatic spine search will only occur in the region"
              " defined by the max spine distance.<br>";
    info+=">> <b>Max spine width:</b> Maximum width allowed for each spine.<br>";
    info+=">> For further questions, please contact Yujie Li at yujie.jade@gmail.com)";

    QTextEdit *text_info = new QTextEdit;
    text_info->setText(info);
    text_info->setReadOnly(true);
    mygridLayout->addWidget(text_info,18,0,1,9);

    this->setLayout(mygridLayout);
    this->setWindowTitle("Spine_detector");
    this->show();
    connect(ok,     SIGNAL(clicked()), this, SLOT(check_data()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_load, SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(btn_swc,SIGNAL(clicked()),this,SLOT(load_swc()));
    connect(btn_csv,SIGNAL(clicked()),this,SLOT(csv_out()));
    //connect(this,SIGNAL(finished(int)),this,SLOT(dialoguefinish(int)));
}

void manual_correct_dialog::check_data()
{
    qDebug()<<"in check data";

    if(edit_load->text().size()==0)
    {
        v3d_msg("You have not provided a valid image");
        return;
    }
    if(edit_swc->text().size()==0)
    {
        v3d_msg("You have not provided a valid swc file");
        return;
    }
    if(edit_csv->text().size()==0)
    {
        v3d_msg("You have not provdied a valid csv output path");
        return; //find the place to write csv
    }
    this->accept();
    get_para();
    if(!auto_spine_detect())
        return;
    if(before_proof_dialog())
        create_standing_dialog();
    else
    {
        return;
    }
}

void manual_correct_dialog::create_standing_dialog()
{
    mydialog=new QDialog;
    mydialog->setWindowTitle("spine proofreading");
    markers=new QComboBox;
    for (int i=0;i<LList_in.size();i++)
        markers->addItem(QString("marker ")+QString::number(i+1));
    markers->setFixedWidth(250);
    markers->setCurrentIndex(0);
    QGridLayout *layout2=new QGridLayout;
    edit_status = new QPlainTextEdit;
    edit_status->setReadOnly(true);
    edit_status->setFixedHeight(50);
    layout2->addWidget(markers,0,0,1,3);
    layout2->addWidget(edit_status,1,0,1,3);

    QPushButton *accept=new QPushButton(tr("Accept"));
    QPushButton *reject=new QPushButton(tr("Delete"));
    QPushButton *skip = new QPushButton(tr("Skip"));

    layout2->addWidget(accept,3,0,1,1);
    layout2->addWidget(reject,3,1,1,1);
    layout2->addWidget(skip,3,2,1,1);

    QPushButton *dilate = new QPushButton(tr("Dilate"));
    QPushButton *erode =new QPushButton (tr("Erode"));
    QPushButton *reset = new QPushButton (tr("Reset"));
    layout2->addWidget(dilate,4,0,1,1);
    layout2->addWidget(erode,4,1,1,1);
    layout2->addWidget(reset,4,2,1,1);

    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    line_1->setFrameShadow(QFrame::Sunken);
    layout2->addWidget(line_1,5,0,1,3);

    QPushButton *button_save=new QPushButton;
    button_save->setText("Finish proofreading");
    QPushButton *button_p_cancel=new QPushButton;
    button_p_cancel->setText("Quit");
    layout2->addWidget(button_save,6,0,1,2);
    layout2->addWidget(button_p_cancel,6,2,1,1);

    mydialog->setLayout(layout2);

    connect(button_save,SIGNAL(clicked()),this,SLOT(finish_proof_dialog()));
    connect(button_p_cancel,SIGNAL(clicked()),this,SLOT(maybe_save()));
    connect(markers,SIGNAL(currentIndexChanged(int)),this,SLOT(marker_roi()));
    connect(accept,SIGNAL(clicked()),this,SLOT(accept_marker()));
    connect(reject,SIGNAL(clicked()),this,SLOT(delete_marker()));
    connect(skip,SIGNAL(clicked()),this,SLOT(skip_marker()));
    connect(dilate,SIGNAL(clicked()),this,SLOT(dilate()));
    connect(erode,SIGNAL(clicked()),this,SLOT(erode()));
    connect(reset,SIGNAL(clicked()),this,SLOT(reset_edit()));

    marker_roi();
    mydialog->show();
}


void manual_correct_dialog::dialoguefinish(int)
{
//    if (this->result()==QDialog::Accepted)
//    {
//        get_para();
//        auto_spine_detect();
//        if(before_proof_dialog())
//        {
//            create_standing_dialog();
//        }
//        else
//            return;
//    }

//    else{
//    //reset image_data and neuron
//        if (image1Dc_in!=0){
//            delete [] image1Dc_in;
//            image1Dc_in=0;
//        }
//        if (image_trun!=0)
//        {
//            delete[] image_trun;
//            image_trun=0;
//        }
//        if (image1Dc_spine!=0)
//        {
//            delete [] image1Dc_spine;
//            image1Dc_spine=0;
//        }
//    }
}

bool manual_correct_dialog::csv_out()
{
    QString fileSaveDir;
    fileSaveDir = QFileDialog::getExistingDirectory(0, QObject::tr("Select Directory to Save Results"),
            "~",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(fileSaveDir.isEmpty())
        return false;

    folder_output = fileSaveDir;

    //fname_output = QDir(folder_output).filePath("spine_profile.csv");
    edit_csv->setText(folder_output);

    QDir::setCurrent(fileSaveDir);
    return true;
}

bool manual_correct_dialog::load_swc()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(0, 0,"","Supported file (*.swc)" ";;Neuron structure(*.swc)",0,0);

    if(filename.isEmpty())
    {
        v3d_msg("No swc file loaded.");
        return false;
    }
    NeuronSWC *p_cur=0;

    neuron = readSWC_file(filename);
    for (V3DLONG ii=0; ii<neuron.listNeuron.size(); ii++)
    {
        p_cur = (NeuronSWC *)(&(neuron.listNeuron.at(ii)));
        if (p_cur->r<0)
        {
            v3d_msg("You have illeagal radius values. Check your data.");
            return false;
        }
     }
    edit_swc->setText(filename);
    qDebug()<<"swc set";
}

void manual_correct_dialog::load_marker()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(0, 0,"","Supported file (*.marker)" ";;Marker file(*.marker)",0,0);

    if(filename.isEmpty())
    {
        v3d_msg("You don't have any marker file open in the main window.");
        return;
    }
    QList<ImageMarker> tmp_list;
    tmp_list=readMarker_file(filename.toAscii());
    for (int i=0;i<tmp_list.size();i++)
    {
        LocationSimple tmp;
        tmp.x=tmp_list.at(i).x;
        tmp.y=tmp_list.at(i).y;
        tmp.z=tmp_list.at(i).z;
        tmp.color.r=tmp.color.g=tmp.color.b=255;
        LList_in.append(tmp);
    }
    edit_marker->setText(filename);
}

bool manual_correct_dialog::loadImage()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(0, QObject::tr("Choose the input image "),
             QDir::currentPath(),QObject::tr("Images (*.raw *.tif *.lsm *.v3dpbd *.v3draw);;All(*)"));

    if (!fileName.isEmpty())
    {
        if (!simple_loadimage_wrapper(*callback, fileName.toStdString().c_str(), image1Dc_in, sz_img, intype))
        {
            QMessageBox::information(0,"","load image "+fileName+" error!");
            return false;
        }
        if (sz_img[3]>3)
        {
            sz_img[3]=3;
            QMessageBox::information(0,"","More than 3 channels were loaded."
                                     "The first 3 channel will be applied for analysis.");
        }

        V3DLONG size_tmp=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
        if(intype==1)
        {
        }
        else if (intype == 2) //V3D_UINT16;
        {
            convert2UINT8((unsigned short*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else if(intype == 4) //V3D_FLOAT32;
        {
            convert2UINT8((float*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else
        {
            QMessageBox::information(0,"","Currently this program only supports UINT8, UINT16, and FLOAT32 data type.");
            return false;
        }
        edit_load->setText(fileName);
        qDebug()<<"Image set"<<sz_img[0]<<":"<<sz_img[1]<<sz_img[2]<<sz_img[3];
        return true;
    }
}

void manual_correct_dialog::get_para()
{
    //obtain all para
    all_para.bgthr=spin_bg_thr->value();
    all_para.max_dis=spin_max_dis->value();
    all_para.intensity_max_pixel=spin_max_pixel->value();
    all_para.min_pixel=spin_min_pixel->value();
    all_para.width_thr=spin_width_thr->value();
    sel_channel=channel_menu->currentIndex();
    all_para.dst_max_pixel=2000;
}

bool manual_correct_dialog::before_proof_dialog()
{
    QMessageBox mybox;
    int size=label_group.size();
    QString info="The automatic spine detection finds "+ QString::number(size)+" spines";
    mybox.setText(info);
    QPushButton *start=mybox.addButton(tr("Start proofreading"),QMessageBox::ActionRole);
    QPushButton *cancel_button=mybox.addButton(QMessageBox::Cancel);

    mybox.setDefaultButton(start);
    mybox.exec();
    //open_main_triview();
     if (mybox.clickedButton() == start) {
         return true;
     } else if (mybox.clickedButton() == cancel_button) {
         QString info="The spine csv profile is saved at "+ edit_csv->text();
         QMessageBox::information(0,"spine_detector",info,QMessageBox::Ok);
         write_spine_profile("automatic_spine_profile.csv");
         open_main_triview();
         callback->setImageName(main_win,"automatic_spine_detector_result");
         callback->setLandmark(main_win,LList_in);
         callback->updateImageWindow(main_win);
         callback->open3DWindow(main_win);
         callback->pushObjectIn3DWindow(main_win);

         //prepare label
         V3DLONG size_page=sz_img[0]*sz_img[1]*sz_img[2];
         unsigned short *label = new unsigned short [size_page*3];
         memset(label,0,size_page*3);
         for(int i=0; i<label_group.size(); i++)
         {
             GetColorRGB(rgb,i);

             GOV tmp = label_group[i];

             for (int j=0; j<tmp.size(); j++)
             {
                 label[tmp.at(j)->pos] = rgb[0];
                 label[tmp.at(j)->pos+size_page]=rgb[1];
                 label[tmp.at(j)->pos+size_page]=rgb[2];
             }
         }

         Image4DSimple image_label;
         image_label.setData((unsigned char*)label,sz_img[0],sz_img[1],sz_img[2],sz_img[3],V3D_UINT16);
         QString name="automatic_spine_detector_label";
         v3dhandle new_win2=callback->newImageWindow(name);
         callback->setImage(new_win2,&image_label);
         callback->setLandmark(new_win2,LList_in);
         callback->updateImageWindow(new_win2);
         callback->open3DWindow(new_win2);
         callback->pushObjectIn3DWindow(new_win2);
         return false;
     }
}

bool manual_correct_dialog::finish_proof_dialog()
{
    QMessageBox msgBox;
    msgBox.setText("Have you finished proofreading?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret==QMessageBox::No)
        return false;
    else if(ret==QMessageBox::Yes)
    {
        int final_landmarks_num;
        final_landmarks_num = save();
        QMessageBox mybox;
        QString info="After proofreading "+ QString::number(final_landmarks_num)+" spines were found\n";
        info+="The spine csv profile is saved at "+ edit_csv->text();
        mybox.information(0,"spine detector",info,QMessageBox::Ok);
        return true;
    }
}

void manual_correct_dialog::loadLabel()
{
//    qDebug()<<"in loadLABEL";
//    QString fileName;
//    fileName = QFileDialog::getOpenFileName(0, QObject::tr("Choose the input image "),
//             QDir::currentPath(),QObject::tr("Images (*.raw *.tif *.lsm *.v3dpbd *.v3draw);;All(*)"));

//    if (!fileName.isEmpty())
//    {
//        unsigned char * label_tmp = 0;
//        if (!simple_loadimage_wrapper(*callback, fileName.toStdString().c_str(), label_tmp, label_sz, label_intype))
//        {
//            QMessageBox::information(0,"","load image "+fileName+" error!");
//            return;
//        }
//        if(label_intype==2)
//            label=(unsigned short *)label_tmp;
//        else{
//            QMessageBox::information(0,"","this label is not unsigned short,error!");
//            return;
//        }
//        if (label_sz[3]>3)
//        {
//            label_sz[3]=3;
//            QMessageBox::information(0,"","More than 3 channels were loaded."
//                                     "The first 3 channel will be applied for analysis.");
//            return;
//        }
//        qDebug()<<"label_type:"<<label_intype<<":"<<label_sz[0]<<label_sz[3];

//        qDebug()<<"finish loading";
//        edit_label->setText(fileName);

//    }
}

bool manual_correct_dialog::auto_spine_detect()
{
    qDebug()<<"~~~~Auto spine detection starts....";
    spine_fun spine_obj(callback,all_para,sel_channel);
    if (!spine_obj.pushImageData(image1Dc_in,sz_img))
        return false;
    spine_obj.pushSWCData(neuron);
    if(!spine_obj.init()){
        v3d_msg("No spine candidates were found. Please check image and swc file");
        return false;
    }
    if(!spine_obj.reverse_dst_grow())
    {
        v3d_msg("No spines candidates were found; Please check image and swc file");
        return false;
    }
    spine_obj.run_intensityGroup();
    spine_obj.conn_comp_nb6();
    LList_in = spine_obj.get_center_landmarks();
    label_group = spine_obj.get_group_label();
    //spine_obj.saveResult();

    //make copy for label_group
    label_group_copy=label_group;

    V3DLONG size_page=sz_img[0]*sz_img[1]*sz_img[2];
    image1Dc_spine = new unsigned char [size_page*3];
    memset(image1Dc_spine,0,size_page*3);
    memcpy(image1Dc_spine,image1Dc_in,size_page);
    for(int i=0; i<label_group.size(); i++)
    {
        GOV tmp = label_group[i];
        for (int j=0; j<tmp.size(); j++)
        {
            image1Dc_spine[tmp.at(j)->pos+size_page]=255;
        }
    }

//    unsigned char * input_image = new unsigned char [size_page *3];
//    memcpy(input_image,image1Dc_spine,size_page*3);

//    Image4DSimple image;
//    image.setData(input_image,sz_img[0],sz_img[1],sz_img[2],3,V3D_UINT8);
//    v3dhandle newwin = callback->newImageWindow("test");
//    callback->setImage(newwin,&image);
//    callback->updateImageWindow(newwin);
//    callback->open3DWindow(newwin);

//    QString fname_output = "test_tmp_label.v3draw";
//    simple_saveimage_wrapper(*callback,fname_output.toStdString().c_str(),(unsigned char *)label,label_sz, 2);

    qDebug()<<"auto spine_detect complete"<<"LList size:"<<LList_in.size();
    return true;
}

void manual_correct_dialog::reset_image_data()
{
    V3DLONG y_offset=sz_img[0];
    V3DLONG z_offset=sz_img[0]*sz_img[1];
    int mid=markers->currentIndex();//start from zero--marker 1

    int halfwindowsize=30;
    //create bounding box

    V3DLONG x,y,z;
    x=LList_in[mid].x-1;
    y=LList_in[mid].y-1;
    z=LList_in[mid].z-1;

    if (x<halfwindowsize) sz[0]=x+halfwindowsize+1;
    else if (x+halfwindowsize>sz_img[0]) sz[0]=sz_img[0]-x+halfwindowsize;
    else sz[0]=2*halfwindowsize+1;

    if (y<halfwindowsize) sz[1]=y+halfwindowsize+1;
    else if (y+halfwindowsize>sz_img[1]) sz[1]=sz_img[1]-y+halfwindowsize;
    else sz[1]=2*halfwindowsize+1;

    if (z<halfwindowsize) sz[2]=z+halfwindowsize+1;
    else if (z+halfwindowsize>sz_img[2]) sz[2]=sz_img[2]-z+halfwindowsize;
    else sz[2]=2*halfwindowsize+1;

    sz[3]=3;

    qDebug()<<"halfsize:"<<halfwindowsize<<" x,y,z:"<<x<<":"<<y<<":"<<z;
    qDebug()<<"sz0,sz1,sz2:"<<sz[0]<<":"<<sz[1]<<":"<<sz[2];
    if (image_trun!=0)
    {
        delete [] image_trun;
        image_trun=0;
    }
    image_trun=new unsigned char[sz[0]*sz[1]*sz[2]*sz[3]];
    memset(image_trun,0,sz[0]*sz[1]*sz[2]*sz[3]);
    qDebug()<<"image_trun allocated";
    //Store the area around the marker in image_trun
    x_start=MAX(0,x-halfwindowsize);
    y_start=MAX(0,y-halfwindowsize);
    z_start=MAX(0,z-halfwindowsize);

    for (V3DLONG dx=x_start;dx<MIN(sz_img[0],x+halfwindowsize+1);dx++){
        for (V3DLONG dy=y_start;dy<MIN(sz_img[1],y+halfwindowsize+1);dy++){
            for (V3DLONG dz=z_start;dz<MIN(sz_img[2],z+halfwindowsize+1);dz++){
                V3DLONG pos=xyz2pos(dx,dy,dz,y_offset,z_offset);
                V3DLONG pos1=xyz2pos(dx-x_start,dy-y_start,dz-z_start,sz[0],sz[0]*sz[1]);
                image_trun[pos1]=image1Dc_spine[pos];
//                if (label[pos]==mid+1)
//                    image_trun[pos1+sz[0]*sz[1]*sz[2]]=255;
            }
        }
    }

    GOV tmp_group=label_group[mid];

    for (int i=0;i<tmp_group.size();i++)
    {
        V3DLONG new_x=tmp_group[i]->x;
        V3DLONG new_y=tmp_group[i]->y;
        V3DLONG new_z=tmp_group[i]->z;
        V3DLONG trun_pos=xyz2pos(new_x-x_start,new_y-y_start,new_z-z_start,sz[0],sz[0]*sz[1]);
        image_trun[trun_pos+sz[0]*sz[1]*sz[2]]=255;
    }

    qDebug()<<"image trun set";
}

void manual_correct_dialog::reset_label_group()
{
    int mid=markers->currentIndex();
    GOV tmp_group=label_group_copy[mid];
    label_group[mid].clear();
    label_group[mid]=tmp_group;

}

void manual_correct_dialog::adjust_LList_to_imagetrun()
{
    LList_adj.clear();
    for (int i=0;i<LList_in.size();i++)
    {
        LocationSimple tmp;
        tmp.x=LList_in[i].x-x_start;
        tmp.y=LList_in[i].y-y_start;
        tmp.z=LList_in[i].z-z_start;
        tmp.color.r=LList_in[i].color.r;
        tmp.color.g=LList_in[i].color.g;
        tmp.color.b=LList_in[i].color.b;
        LList_adj.append(tmp);
    }
}

void manual_correct_dialog::marker_roi()
{
    qDebug()<<"in marker roi";
    if(markers->count()==0) return;
    edit_status->clear();
    V3DLONG size_tmp=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
    V3DLONG size_page=sz_img[0]*sz_img[1]*sz_img[2];
    int mid=markers->currentIndex();
    prev_idx=mid;

    //check whether previous editing needs saving
    if (edit_flag) save_edit();
    edit_flag=false;

    //step 1: check whether main-triview is open
    open_main_triview();

    //step 2:set data ready and open local tri-view/3d window
    reset_image_data();
    adjust_LList_to_imagetrun();

    //step 3: check whether local triview is open
    check_window();

    //Step 4: Focus on this marker on tri-view
    TriviewControl * p_control = callback->getTriviewControl(curwin);
    p_control->setFocusLocation((long)LList_in.at(mid).x,
                                (long)LList_in.at(mid).y,(long)LList_in.at(mid).z);

    //Step 5: update marker in 2 tri-view and one 3D
    // if this markers is not determined,Landmark color change
    if (LList_in[mid].comments.empty())
    {
        LList_in[mid].color.r=LList_in[mid].color.b=255;
        LList_adj[mid].color.r=LList_adj[mid].color.b=255;
        LList_in[mid].color.g=70;
        LList_adj[mid].color.g=70;
    }
    Image4DSimple image4d;
    unsigned char * image_input=new unsigned char [sz[0]*sz[1]*sz[2]*sz[3]];
    memcpy(image_input,image_trun,sz[0]*sz[1]*sz[2]*sz[3]);
    image4d.setData(image_input,sz[0],sz[1],sz[2],sz[3],V3D_UINT8);
    callback->setImage(curwin,&image4d);
    callback->setLandmark(curwin,LList_adj);
    callback->updateImageWindow(curwin);
    callback->close3DWindow(curwin);
    callback->open3DWindow(curwin);
    callback->pushObjectIn3DWindow(curwin);
    callback->setLandmark(main_win,LList_in);

    //Step 6: reset marker color back to original color
    LList_in[mid].color.r=LList_in[mid].color.b=LList_in[mid].color.g=255;
    LList_adj[mid].color.r=LList_adj[mid].color.b=LList_in[mid].color.g=255;
    qDebug()<<"~~~~marker roi finished";
}

void manual_correct_dialog::skip_marker()
{
    open_main_triview();
    check_window();
    int i=markers->currentIndex();
    if(i+1<markers->count())
        markers->setCurrentIndex(i+1);
}

void manual_correct_dialog::accept_marker()
{
    qDebug()<<"in accept marker";
    open_main_triview();
    check_window();

    //update landmarks color and status
    int mid=markers->currentIndex();
    prev_idx=mid;
    LList_in[mid].color.r=LList_in[mid].color.b=0;
    LList_in[mid].color.g=255;
    LList_adj[mid].color.r=LList_in[mid].color.b=0;
    LList_adj[mid].color.g=255;

    QString tmp=QString::number(1);
    LList_in[mid].comments=tmp.toStdString(); //accepted marker
    callback->setLandmark(curwin,LList_in);

    //update Qcombobox
    markers->setItemText(mid, "Marker: " + QString::number(mid+1) + " accepted");
    //edit_status->setPlainText("Marker: " + QString::number(mid+1) + " accepted");

    edit_flag=false;
    //go to next marker
    if(mid+1<markers->count()){
        markers->setCurrentIndex(mid+1);
    }
}

void manual_correct_dialog::delete_marker()
{
    open_main_triview();
    check_window();
    qDebug()<<"delete marker";
    //update landmarks color and status
    int mid=markers->currentIndex();

    LList_in[mid].color.g=LList_in[mid].color.b=0;
    LList_in[mid].color.r=255;
    LList_adj[mid].color.g=LList_in[mid].color.b=0;
    LList_adj[mid].color.r=255;

    LList_in[mid].comments=QString::number(2).toStdString(); //rejected marker

    //update combobox
    markers->setItemText(mid, "Marker: " + QString::number(mid+1) + " rejected");
    //edit_status->setPlainText("Marker: " + QString::number(mid+1) + " rejected");
    edit_flag=false;

    if(mid+1<markers->count()){
        markers->setCurrentIndex(mid+1);
    }
}

void manual_correct_dialog::open_main_triview()
{
    qDebug()<<"open main_triview window";
    bool window_open_flag=false;
    v3dhandleList list_triwin = callback->getImageWindowList();
    for(V3DLONG i=0; i<list_triwin.size(); i++){
        if(callback->getImageName(list_triwin.at(i)).contains(main_win_name))
        {
            window_open_flag=true;
            main_win=list_triwin[i];
            return;
        }
    }
    if(!window_open_flag)
    {
        main_win = callback->newImageWindow(main_win_name);
        unsigned char *image_input=new unsigned char [sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3]];
        memcpy(image_input,image1Dc_spine,sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3]);
        Image4DSimple image_main;
        image_main.setData(image_input,sz_img[0],sz_img[1],sz_img[2],sz_img[3],V3D_UINT8);
        callback->setImage(main_win,&image_main);
        callback->updateImageWindow(main_win);
    }
}

void manual_correct_dialog::check_window()
{
    bool window_open_flag=false;
    v3dhandleList list_triwin = callback->getImageWindowList();
    for(V3DLONG i=0; i<list_triwin.size(); i++){
        if(callback->getImageName(list_triwin.at(i)).contains(fname_image))
        {
            window_open_flag=true;
            curwin=list_triwin[i];
            break;
        }
    }
    qDebug()<<"check window: window_open_flag:"<<window_open_flag;
    if(!window_open_flag)
    {
        curwin=callback->newImageWindow(fname_image);
        unsigned char *image_input=new unsigned char [sz[0]*sz[1]*sz[2]*sz[3]];
        memcpy(image_input,image_trun,sz[0]*sz[1]*sz[2]*sz[3]);
        Image4DSimple image_tmp;
        image_tmp.setData(image_input,sz[0],sz[1],sz[2],sz[3],V3D_UINT8);
        callback->setImage(curwin,&image_tmp);
        callback->updateImageWindow(curwin);
    }
}

void manual_correct_dialog::check_local_3d_window()
{
    bool window_open_flag=false;
    QList <V3dR_MainWindow *> allWindowList = callback->getListAll3DViewers();
    for (V3DLONG i=0;i<allWindowList.size();i++)
    {
        if(callback->getImageName(allWindowList.at(i)).contains(fname_image) &&
               callback->getImageName((allWindowList.at(i))).contains("Local"))
        {
            window_open_flag=true;
            break;
        }
    }

    if (!window_open_flag)
    {
        callback->openROI3DWindow(curwin);
        View3DControl * v3dlocalcontrol = callback->getLocalView3DControl(curwin);
        v3dlocalcontrol->updateLandmark();
    }
}

int manual_correct_dialog::save()
{
    open_main_triview();
    //prepare Landmarkers and image
    LandmarkList LList_new;

    V3DLONG size_page=sz_img[0]*sz_img[1]*sz_img[2];
    //prepare label
    unsigned short *label = new unsigned short [size_page*3];
    memset(label,0,size_page*3);
    memset(image1Dc_spine+size_page,0,size_page);

    for (int i=0;i<LList_in.size();i++)
    {
        QString tmp;
        tmp=QString::fromStdString(LList_in[i].comments);
        if (tmp.contains("2")) //reject
        {
//            GOV tmp_group=label_group[i];
//            for (int j=0;j<tmp_group.size();j++)
//            {
//                image1Dc_spine[tmp_group[j]->pos +size_page]=0;
//            }
        }
        else if (tmp.contains("1"))
        {
            GOV tmp_group=label_group[i];
            GetColorRGB(rgb,i);
            int sum_x,sum_y,sum_z;
            sum_x=sum_y=sum_z=0;
            for (int j=0;j<tmp_group.size();j++)
            {
                image1Dc_spine[tmp_group[j]->pos+size_page]=255;
                sum_x+=tmp_group[j]->x;
                sum_y+=tmp_group[j]->y;
                sum_z+=tmp_group[j]->z;

                label[tmp_group.at(j)->pos] = rgb[0];
                label[tmp_group.at(j)->pos+size_page]=rgb[1];
                label[tmp_group.at(j)->pos+size_page]=rgb[2];
            }
            LocationSimple tmp;
            tmp.x=sum_x/tmp_group.size();
            tmp.y=sum_y/tmp_group.size();
            tmp.z=sum_z/tmp_group.size();
            tmp.color.r=tmp.color.b=0;
            tmp.color.g=255;
            LList_new.append(tmp);
        }
    }

    //finish_proof_dialog(LList_new.size());
    write_spine_profile("automatic_manual_proof_spine_profile.csv");

    //need to close all image windows //check 3D window
    v3dhandleList list_triwin = callback->getImageWindowList();
    for(V3DLONG i=0; i<list_triwin.size(); i++){
        if(callback->getImageName(list_triwin.at(i)).contains(fname_image))
        {
            callback->close3DWindow(list_triwin[i]);
        }
    }

    mydialog->close();

    //visualize
    Image4DSimple image4d;
    unsigned char *input_image=new unsigned char [size_page*sz_img[3]];
    memcpy(input_image,image1Dc_spine,size_page*sz_img[3]);
    image4d.setData(input_image,sz_img[0],sz_img[1],sz_img[2],sz_img[3],V3D_UINT8);
    QString final_name="proofread_image";
    callback->setImage(curwin,&image4d);
    callback->setImageName(curwin,final_name);
    callback->setLandmark(curwin,LList_new);
    callback->updateImageWindow(curwin);
    callback->open3DWindow(curwin);
    callback->pushObjectIn3DWindow(curwin);

    Image4DSimple image_label;
    image_label.setData((unsigned char*)label,sz_img[0],sz_img[1],sz_img[2],sz_img[3],V3D_UINT16);
    QString name="proofread_image_label";
    v3dhandle new_win2=callback->newImageWindow(name);
    callback->setImage(new_win2,&image_label);
    callback->setLandmark(new_win2,LList_new);
    callback->updateImageWindow(new_win2);
    callback->open3DWindow(new_win2);
    callback->pushObjectIn3DWindow(new_win2);
    return LList_new.size();

}


bool manual_correct_dialog::maybe_save()
{
    QMessageBox mybox;
    mybox.setText("Do you want to exit without saving?");

    QPushButton *save_button = mybox.addButton(QMessageBox::Save);
    QPushButton *cancel_button=mybox.addButton(QMessageBox::Cancel);
    QPushButton *discard_button=mybox.addButton(QMessageBox::Discard);

    mybox.setDefaultButton(QMessageBox::Save);
    mybox.exec();

     if (mybox.clickedButton() == save_button) {
         save();
         return true;
     } else if (mybox.clickedButton() == cancel_button) {
         return false;
     }
     else if (mybox.clickedButton()== discard_button) {
         //need to close all image windows //check 3D window
         v3dhandleList list_triwin = callback->getImageWindowList();
         for(V3DLONG i=0; i<list_triwin.size(); i++){
             if(callback->getImageName(list_triwin.at(i)).contains(fname_image))
             {
                 callback->close3DWindow(list_triwin[i]);
             }
         }
         mydialog->close();
         return false;
     }
}

void manual_correct_dialog::reject()
{
    qDebug()<<"reject working";
    finish();
    return;
}

void manual_correct_dialog::write_spine_profile(QString filename)
{
    qDebug()<<"in write spine center profile";

    QString outfile=edit_csv->text()+filename;
    FILE *fp2=fopen(outfile.toAscii(),"wt");
    fprintf(fp2,"##id,volume,max_dis,min_dis,center_dis,center_x,center_y,center_z\n");
    for (int i=0;i<label_group.size();i++)
    {
        GOV tmp=label_group[i];
        sort(tmp.begin(),tmp.end(),sortfunc_dst);
        int group_id=tmp.front()->intensity_label;
        int max_dis=tmp.front()->dst;
        int min_dis=tmp.back()->dst;
        int volume=tmp.size();
        V3DLONG sum_x,sum_y,sum_z,sum_dis;
        sum_x=sum_y=sum_z=sum_dis=0;
        for (int j=0;j<tmp.size();j++)
        {
            sum_x+=tmp[j]->x;
            sum_y+=tmp[j]->y;
            sum_z+=tmp[j]->z;
            sum_dis+=tmp[j]->dst;
        }
        int center_x=sum_x/tmp.size();
        int center_y=sum_y/tmp.size();
        int center_z=sum_z/tmp.size();
        int center_dis=sum_dis/tmp.size();
        fprintf(fp2,"%d,%d,%d,%d,%d,%d,%d,%d\n",group_id,volume,max_dis,
                min_dis,center_dis,center_x,center_y,center_z);

    }
    fclose(fp2);
}

void manual_correct_dialog::finish()
{
    if (!maybe_save())
    {
        //need to close all image windows //check 3D window
        v3dhandleList list_triwin = callback->getImageWindowList();
        for(V3DLONG i=0; i<list_triwin.size(); i++){
            if(callback->getImageName(list_triwin.at(i)).contains(fname_image))
            {
                callback->close3DWindow(list_triwin.at(i));
                callback->closeROI3DWindow(list_triwin.at(i));
            }
        }
        mydialog->close();
    }
    else
        return;
}

void manual_correct_dialog::dilate()
{
    open_main_triview();
    check_window();
    //qDebug()<<"in dilate now";
    int bg_thr=80;
    V3DLONG size_page=sz[0]*sz[1]*sz[2];
    GOV seeds_next;

    int mid=markers->currentIndex();
    GOV tmp_group = label_group[mid];

    for (int sid=0;sid<tmp_group.size();sid++)
    {
        VOI * single_voi = tmp_group[sid];
        int label_id=single_voi->intensity_label;
        for (int neid=0; neid<single_voi->neighbors_6.size();neid++)
        {
            if (image1Dc_spine[single_voi->neighbors_6[neid]->pos]>bg_thr &&
                    single_voi->neighbors_6[neid]->intensity_label<1)
            {
                single_voi->neighbors_6[neid]->intensity_label=label_id;
                seeds_next.push_back(single_voi->neighbors_6[neid]);
            }
        }
    }

    if (seeds_next.size()==0)
    {
        edit_status->setPlainText("No more voxels available. Cannot dilate");
        return;
    }
    else if (seeds_next.size()>0)
    {
        edit_status->setPlainText(QString::number(seeds_next.size())+" voxels added in this round of dilation");
        tmp_group.insert(tmp_group.end(),seeds_next.begin(),seeds_next.end());
        label_group[mid].clear();
        label_group[mid]=tmp_group;
    }

    for (int i=0;i<seeds_next.size();i++)
    {
        VOI * single_voi=seeds_next[i];
        V3DLONG pos_trun=xyz2pos(single_voi->x-x_start,single_voi->y-y_start,single_voi->z-z_start
                                 ,sz[0],sz[0]*sz[1]);
        image_trun[pos_trun+size_page]=255;
        //image_trun[pos_trun+2*size_page]=255;
    }

    unsigned char *dilate_tmp =new unsigned char [sz[0]*sz[1]*sz[2]*sz[3]];
    memcpy(dilate_tmp,image_trun,sz[0]*sz[1]*sz[2]*sz[3]);
    edit_flag=true;

    Image4DSimple image4d_tmp;
    image4d_tmp.setData(dilate_tmp,sz[0],sz[1],sz[2],sz[3],V3D_UINT8);
    callback->setImage(curwin,&image4d_tmp);
    callback->updateImageWindow(curwin);
    callback->close3DWindow(curwin);
    callback->setLandmark(curwin,LList_adj);
    callback->open3DWindow(curwin);
    callback->pushObjectIn3DWindow(curwin);
}

void manual_correct_dialog::erode()
{
    int bg_thr=80;
    open_main_triview();
    check_window();
    qDebug()<<"in erode now";
    V3DLONG size_page=sz[0]*sz[1]*sz[2];

    int mid=markers->currentIndex();
    GOV tmp_group = label_group[mid];

    if (tmp_group.size()==0)
    {
        edit_status->setPlainText("No more voxels left. Cannot erode");
        return;
    }
    sort(tmp_group.begin(),tmp_group.end(),sortfunc_dst_ascend); //ascending
    //qDebug()<<"sort done:"<<"size:"<<tmp_group.size();

    int min_dis=tmp_group.front()->dst;
    int vid_begin, vid_end;
    vid_begin=vid_end=0;

    while(vid_end<tmp_group.size() && tmp_group[vid_end]->dst==min_dis){
        vid_end++;
        continue;
    }
    if (vid_begin==vid_end)
    {
       edit_status->setPlainText("No more voxels left. Cannot erode");
       return;
    }

    for (int i=vid_begin;i<vid_end;i++)
    {
        VOI * single_voi= tmp_group[i];
        V3DLONG trun_pos= xyz2pos(single_voi->x-x_start ,single_voi->y-y_start,
                                  single_voi->z-z_start,sz[0],sz[0]*sz[1]);
        image_trun[trun_pos+size_page]=0;
    }
    edit_status->setPlainText(QString::number(vid_end-vid_begin)+" voxels deleted in this round of erosion");
    //qDebug()<<"~~~erode~~~~decrease:"<<vid_end-vid_begin;

    GOV update_group;
    for (int i=vid_end;i<tmp_group.size();i++)
    {
        update_group.push_back(tmp_group[i]);
    }

    label_group[mid].clear();
    label_group[mid]=update_group;
    //qDebug()<<"new group update";
    unsigned char *erode_tmp =new unsigned char [sz[0]*sz[1]*sz[2]*sz[3]];
    memcpy(erode_tmp,image_trun,sz[0]*sz[1]*sz[2]*sz[3]);
    edit_flag=true;

    Image4DSimple image4d_tmp;
    image4d_tmp.setData(erode_tmp,sz[0],sz[1],sz[2],sz[3],V3D_UINT8);
    callback->setImage(curwin,&image4d_tmp);
    callback->updateImageWindow(curwin);
    callback->close3DWindow(curwin);
    callback->setLandmark(curwin,LList_adj);
    callback->open3DWindow(curwin);
    callback->pushObjectIn3DWindow(curwin);
}

void manual_correct_dialog::reset_edit()
{
    reset_label_group();
    reset_image_data();
    edit_flag=false;
    unsigned char *reset_tmp =new unsigned char [sz[0]*sz[1]*sz[2]*sz[3]];
    memcpy(reset_tmp,image_trun,sz[0]*sz[1]*sz[2]*sz[3]);
    edit_status->setPlainText("Image reset");

    Image4DSimple image4d_tmp;
    image4d_tmp.setData(reset_tmp,sz[0],sz[1],sz[2],sz[3],V3D_UINT8);
    callback->setImage(curwin,&image4d_tmp);
    callback->updateImageWindow(curwin);
    callback->close3DWindow(curwin);
    callback->setLandmark(curwin,LList_adj);
    callback->open3DWindow(curwin);
    callback->pushObjectIn3DWindow(curwin);

}


bool manual_correct_dialog::save_edit()
{
    QMessageBox mybox;
    mybox.setText("Click accept after editing to save. Otherwise your edit will be discarded");
    QPushButton *accept_button=mybox.addButton(tr("Accept"),QMessageBox::ActionRole);
    QPushButton *discard_button=mybox.addButton(QMessageBox::Discard);
    QPushButton *cancel_button=mybox.addButton(QMessageBox::Cancel);

    mybox.setDefaultButton(accept_button);
    mybox.exec();

     if (mybox.clickedButton() == accept_button) {
         accept_marker();
         return true;
     } else if (mybox.clickedButton() == cancel_button) {
         return false;
     } else if (mybox.clickedButton() == discard_button) {
         reset_image_data();
         reset_label_group();
         edit_flag=false;
         return false;
     }
}

void manual_correct_dialog::GetColorRGB(int* rgb, int idx)
{
    idx=idx+1;
    if(idx>=0){
    idx = idx > 0 ? idx % 128 : 128 - abs(idx % 128);
    int colorLib[128][3] = {
            {39,16,2}, //{55, 173, 188},
            {3, 91, 61},
            {237, 195, 97},
            {175, 178, 151},
            {245, 199, 137},
            {24, 226, 66},
            {118, 84, 193},
            {205, 205, 224},
            {22, 170, 128},
            {86, 150, 90},
            {53, 67, 25},
            {38, 82, 152},
            {55,173,188}, //{39, 16, 2},
            {197, 174, 31},
            {183, 41, 31},
            {174, 37, 15},
            {34, 4, 48},
            {18, 10, 79},
            {132, 100, 254},
            {112, 39, 198},
            {189, 22, 120},
            {75, 104, 110},
            {222, 180, 4},
            {6, 60, 153},
            {236, 85, 113},
            {25, 182, 184},
            {200, 240, 12},
            {204, 119, 71},
            {11, 201, 229},
            {198, 214, 88},
            {49, 97, 65},
            {5, 75, 61},
            {52, 89, 137},
            {32, 49, 125},
            {104, 187, 76},
            {206, 233, 195},
            {85, 183, 9},
            {157, 29, 13},
            {14, 7, 244},
            {134, 193, 150},
            {188, 81, 216},
            {126, 29, 207},
            {170, 97, 183},
            {107, 14, 149},
            {76, 104, 149},
            {80, 38, 253},
            {27, 85, 10},
            {235, 95, 252},
            {139, 144, 48},
            {55, 124, 56},
            {239, 71, 243},
            {208, 89, 6},
            {87, 98, 24},
            {247, 48, 42},
            {129, 130, 13},
            {94, 149, 254},
            {111, 177, 4},
            {39, 229, 12},
            {7, 146, 87},
            {56, 231, 174},
            {95, 102, 52},
            {61, 226, 235},
            {199, 62, 149},
            {51, 32, 175},
            {232, 191, 210},
            {57, 99, 107},
            {239, 27, 135},
            {158, 71, 50},
            {104, 92, 41},
            {228, 112, 171},
            {54, 120, 13},
            {126, 69, 174},
            {191, 100, 143},
            {187, 156, 148},
            {18, 95, 9},
            {104, 168, 147},
            {249, 113, 198},
            {145, 5, 131},
            {104, 56, 59},
            {112, 235, 81},
            {73, 93, 127},
            {207, 60, 6},
            {77, 76, 211},
            {35, 208, 220},
            {141, 5, 129},
            {182, 178, 228},
            {179, 239, 3},
            {209, 9, 132},
            {167, 192, 71},
            {154, 227, 164},
            {200, 125, 103},
            {224, 181, 211},
            {3, 24, 139},
            {218, 67, 198},
            {163, 185, 228},
            {196, 53, 11},
            {39, 183, 33},
            {56, 116, 119},
            {23, 129, 20},
            {42, 191, 85},
            {78, 209, 250},
            {247, 147, 60},
            {74, 172, 146},
            {51, 244, 86},
            {38, 84, 29},
            {197, 202, 150},
            {20, 49, 198},
            {90, 214, 34},
            {178, 49, 101},
            {86, 235, 214},
            {161, 221, 55},
            {17, 173, 136},
            {132, 65, 217},
            {210, 4, 121},
            {241, 117, 217},
            {137, 111, 6},
            {129, 224, 232},
            {73, 34, 0},
            {81, 135, 211},
            {172, 233, 193},
            {43, 246, 89},
            {153, 187, 222},
            {85, 118, 43},
            {119, 116, 33},
            {163, 229, 109},
            {45, 75, 15},
            {15, 7, 140},
            {144, 78, 192}
    };
    for(int k = 0; k < 3; k++)
        rgb[k] = colorLib[idx][k];
    }
    else{
        rgb[0]=0;
        rgb[1]=0;
        rgb[2]=0;
    }
}