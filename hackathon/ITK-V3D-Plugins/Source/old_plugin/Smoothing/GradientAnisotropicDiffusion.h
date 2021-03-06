#ifndef __GradientAnisotropicDiffusion_H__
#define __GradientAnisotropicDiffusion_H__

#include "V3DITKPluginDefaultHeader.h"

class GradientAnisotropicDiffusionPlugin : public QObject, public V3DPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(V3DPluginInterface)
  V3DITKPLUGIN_DEFAULT_CLASS_DECLARATION_BODY(GradientAnisotropicDiffusion);
};

#endif
