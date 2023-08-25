// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MARGINSINPUT_H
#define BLACKGUI_COMPONENTS_MARGINSINPUT_H

#include <QFrame>
#include <QMargins>
#include <QScopedPointer>

namespace Ui
{
    class CMarginsInput;
}

namespace BlackGui::Components
{
    /*!
     * Widget alows to enter margins
     */
    class CMarginsInput : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMarginsInput(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CMarginsInput() override;

        //! Set margins
        void setMargins(const QMargins &margins);

        //! Current values of margins
        QMargins getMargins() const;

    signals:
        //! Margins changed
        void changedMargins(const QMargins &margins);

    private:
        //! Ok
        void confirmed();

        QScopedPointer<Ui::CMarginsInput> ui;
    };
} // ns

#endif // guard
