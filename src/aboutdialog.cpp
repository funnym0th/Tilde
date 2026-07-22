#include "aboutdialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QFont>
#include <QTabWidget>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("About Tilde");
    setWindowIcon(QIcon::fromTheme("tilde", QIcon("res/tilde.svg")));
    setFixedSize(460, 380);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Header section with icon and title
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* iconLabel = new QLabel(this);
    iconLabel->setPixmap(QIcon::fromTheme("tilde", QIcon("res/tilde.svg")).pixmap(64, 64));
    headerLayout->addWidget(iconLabel);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("<b>Tilde</b>", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleLabel->setFont(titleFont);

    QLabel* versionLabel = new QLabel("Version 0.1.0", this);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(versionLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Tabs for structured clean info
    QTabWidget* tabWidget = new QTabWidget(this);

    // Tab 1: About
    QLabel* aboutLabel = new QLabel(
        "<p>A native C++ document editor for Markdown and LaTeX built on Qt 6 and the KDE KTextEditor engine.</p>"
        "<p>It features real-time live rendering, synchronized split scrolling, and direct PDF compilation natively.</p>",
        this);
    aboutLabel->setWordWrap(true);
    aboutLabel->setOpenExternalLinks(true);
    aboutLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    aboutLabel->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(aboutLabel, "About");

    // Tab 2: Author & Credits
    QLabel* authorLabel = new QLabel(
        "<p><b>Created and Maintained by:</b><br>"
        "Camilo Catalán (<i>funnym0th</i>)</p>"
        "<p><b>GitHub Repository:</b><br>"
        "<a href=\"https://github.com/funnym0th/Tilde\">https://github.com/funnym0th/Tilde</a></p>",
        this);
    authorLabel->setWordWrap(true);
    authorLabel->setOpenExternalLinks(true);
    authorLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    authorLabel->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(authorLabel, "Author");

    // Tab 3: License
    QLabel* licenseLabel = new QLabel(
        "<p><b>GPL-3.0-only License</b></p>"
        "<p>This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, version 3 of the License.</p>",
        this);
    licenseLabel->setWordWrap(true);
    licenseLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    licenseLabel->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(licenseLabel, "License");

    mainLayout->addWidget(tabWidget);

    // Close button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton* closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
}
