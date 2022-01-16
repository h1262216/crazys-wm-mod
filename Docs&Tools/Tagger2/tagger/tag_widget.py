from PySide6.QtCore import Qt, QStringListModel
from PySide6.QtWidgets import QDialog, QComboBox, QVBoxLayout, QLineEdit, QWidget, QGroupBox, QCheckBox, QHBoxLayout, \
    QGridLayout, QListWidget, QLabel, QPushButton, QTabWidget, QScrollArea, QCompleter
from collections import defaultdict

from typing import Set, List
from .resource import ImageResource


class TagViewWidget(QWidget):
    def __init__(self, repo):
        super().__init__()
        self.repo = repo
        self.edit = QLineEdit()

        layout = QVBoxLayout()
        self.edit.editingFinished.connect(self.on_update_type)

        model = QStringListModel()
        options = set()
        for s in self.repo.values():
            options.add(s.tag)
            options.add(s.display)

        model.setStringList(options)
        completer = QCompleter()
        completer.setModel(model)
        self.edit.setCompleter(completer)
        layout.addWidget(QLabel("Image Type"))
        layout.addWidget(self.edit)
        self.display_type = QLabel("")
        layout.addWidget(self.display_type)

        layout.addWidget(QLabel("Outfit"))
        self.outfit = QComboBox()
        self.outfit.addItems(["Armour", "Fetish", "Maid", "Teacher", "Nurse", "Formal", "Schoolgirl", "Swimwear", "Lingerie",
                              "Farmer", "Sorceress", "Rags", "Casual", "Nude", "None"])
        layout.addWidget(self.outfit)

        # pregnant variation
        layout.addWidget(QLabel("Flags"))
        self.preg = QCheckBox()
        self.preg.setText("Pregnant")
        layout.addWidget(self.preg)
        self.fallback = QCheckBox()
        self.fallback.setText("Fallback")
        layout.addWidget(self.fallback)

        self.setLayout(layout)

    def on_update_type(self):
        text = self.edit.text().strip()
        if text in self.repo:
            self.display_type.setText(self.repo[text].display)
        else:
            found = False
            for entry in self.repo.values():
                if entry.display == text:
                    self.edit.setText(entry.tag)
                    self.display_type.setText(text)
                    found = True
                    break
            if not found:
                self.display_type.setText("ERROR")
        self.preg.setFocus()

    def update_resource(self, resource: ImageResource):
        self.edit.setText(resource.type)
        try:
            self.display_type.setText(self.repo[resource.type].display)
        except KeyError:
            self.display_type.setText("ERROR" if resource.type != "" else "<NONE>")
        self.preg.setCheckState(Qt.Checked if resource.pregnant else Qt.Unchecked)
        self.fallback.setCheckState(Qt.Checked if resource.fallback else Qt.Unchecked)
        self.outfit.setCurrentText(resource.outfit.title())

    def write_changes(self, resource: ImageResource):
        resource.type = self.edit.text()
        resource.pregnant = self.preg.checkState() == Qt.Checked
        resource.fallback = self.fallback.checkState() == Qt.Checked
        resource.outfit = self.outfit.currentText().lower()
