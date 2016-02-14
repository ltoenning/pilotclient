/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "audiosetupcomponent.h"
#include "ui_audiosetupcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextaudio.h"
#include "blackmisc/logmessage.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Audio::Settings;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CAudioSetupComponent::CAudioSetupComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioSetupComponent)
        {
            ui->setupUi(this);

            bool c = connect(this->ui->tb_ExpandNotificationSounds, &QToolButton::toggled, this, &CAudioSetupComponent::ps_onToggleNotificationSoundsVisibility);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(this->ui->cb_SetupAudioLoopback, &QCheckBox::toggled, this, &CAudioSetupComponent::ps_onLoopbackToggled);
            Q_ASSERT(c);
            Q_UNUSED(c);

            if (sGui->getIContextAudio())
            {
                this->initAudioDeviceLists();

                // default
                this->ui->cb_SetupAudioLoopback->setChecked(sGui->getIContextAudio()->isAudioLoopbackEnabled());

                // the connects depend on initAudioDeviceLists
                c = this->connect(this->ui->cb_SetupAudioInputDevice,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::ps_audioDeviceSelected);
                Q_ASSERT(c);
                Q_UNUSED(c);

                c = this->connect(this->ui->cb_SetupAudioOutputDevice, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::ps_audioDeviceSelected);
                Q_ASSERT(c);
                Q_UNUSED(c);

                // context
                this->connect(sGui->getIContextAudio(), &IContextAudio::changedAudioDevices, this, &CAudioSetupComponent::ps_onAudioDevicesChanged);
                this->connect(sGui->getIContextAudio(), &IContextAudio::changedSelectedAudioDevices, this, &CAudioSetupComponent::ps_onCurrentAudioDevicesChanged);
            }
            this->ps_reloadSettings();
            this->ui->tb_ExpandNotificationSounds->setChecked(false); // collapse
        }

        CAudioSetupComponent::~CAudioSetupComponent()
        { }

        void CAudioSetupComponent::ps_reloadSettings()
        {
            CSettingsAudio as(m_audioSettings.get());
            this->ui->cb_SetupAudioPlayNotificationSounds->setChecked(true);
            this->ui->cb_SetupAudioNotificationTextMessage->setChecked(as.getNotificationFlag(CNotificationSounds::NotificationTextMessagePrivate));
            this->ui->cb_SetupAudioNotificationVoiceRoom->setChecked(as.getNotificationFlag(CNotificationSounds::NotificationVoiceRoomJoined));
        }

        void CAudioSetupComponent::ps_onToggleNotificationSoundsVisibility(bool checked)
        {
            this->ui->fr_NotificationSoundsInner->setVisible(checked);
        }

        void CAudioSetupComponent::initAudioDeviceLists()
        {
            if (!sGui->getIContextAudio()) { return; }
            this->ps_onAudioDevicesChanged(sGui->getIContextAudio()->getAudioDevices());
            this->ps_onCurrentAudioDevicesChanged(sGui->getIContextAudio()->getCurrentAudioDevices());
        }

        bool CAudioSetupComponent::playNotificationSounds() const
        {
            return this->ui->cb_SetupAudioPlayNotificationSounds->isChecked();
        }

        void CAudioSetupComponent::ps_audioDeviceSelected(int index)
        {
            if (!sGui->getIContextAudio()) return;
            if (index < 0) { return; }

            CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) { return; }
            CAudioDeviceInfo selectedDevice;
            QObject *sender = QObject::sender();
            if (sender == this->ui->cb_SetupAudioInputDevice)
            {
                CAudioDeviceInfoList inputDevices = devices.getInputDevices();
                if (index >= inputDevices.size()) { return; }
                selectedDevice = inputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
            else if (sender == this->ui->cb_SetupAudioOutputDevice)
            {
                CAudioDeviceInfoList outputDevices = devices.getOutputDevices();
                if (index >= outputDevices.size()) { return; }
                selectedDevice = outputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
        }

        void CAudioSetupComponent::ps_onCurrentAudioDevicesChanged(const CAudioDeviceInfoList &devices)
        {
            for (auto &device : devices)
            {
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    this->ui->cb_SetupAudioInputDevice->setCurrentText(device.toQString(true));
                }
                else if (device.getType() == CAudioDeviceInfo::OutputDevice)
                {
                    this->ui->cb_SetupAudioOutputDevice->setCurrentText(device.toQString(true));
                }
            }
        }

        void CAudioSetupComponent::ps_onAudioDevicesChanged(const CAudioDeviceInfoList &devices)
        {
            this->ui->cb_SetupAudioOutputDevice->clear();
            this->ui->cb_SetupAudioInputDevice->clear();

            for (auto &device : devices)
            {
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    this->ui->cb_SetupAudioInputDevice->addItem(device.toQString(true));
                }
                else if (device.getType() == CAudioDeviceInfo::OutputDevice)
                {
                    this->ui->cb_SetupAudioOutputDevice->addItem(device.toQString(true));
                }
            }
        }

        void CAudioSetupComponent::ps_onLoopbackToggled(bool loopback)
        {
            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->isAudioLoopbackEnabled() == loopback) { return; }
            sGui->getIContextAudio()->enableAudioLoopback(loopback);
        }

    } // namespace
} // namespace
