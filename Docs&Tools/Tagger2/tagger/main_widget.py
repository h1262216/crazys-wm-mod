from typing import Optional, List

from PySide6.QtGui import QImage, QPixmap, QImageReader
from PySide6.QtWidgets import QWidget, QLabel, QVBoxLayout, QHBoxLayout, QListWidget, QFrame, QPushButton, \
    QListWidgetItem, QGridLayout, QMenuBar, QCheckBox, QComboBox, QLineEdit, QFormLayout
from PySide6.QtCore import Qt

from .resource import ImageResource, ResourcePack
from .tag_widget import TagViewWidget

from pathlib import Path


class MainWidget(QWidget):
    def __init__(self, image_width, image_height, repo: dict):
        super().__init__()

        self.image_width = image_width
        self.image_height = image_height

        self.repo = repo
        layout = QGridLayout()

        # pack image list
        self.pack_list = QListWidget()
        self.pack_list.setMinimumWidth(256)
        self.pack_list.currentRowChanged.connect(self._select_image)
        layout.addWidget(self.pack_list, 0, 0, 1, 1)

        # the image display
        self.pixmap = QPixmap()
        self.label = QLabel("Active Image")
        self.label.setMinimumWidth(self.image_width)
        self.label.setMinimumHeight(self.image_height)
        self.label.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        layout.addWidget(self.label, 0, 1, 2, 1)

        image_meta = QFormLayout()
        layout.addLayout(image_meta, 1, 0, 1, 1)

        #self.fallback = QCheckBox("Fallback")
        #self.fallback.setTristate(False)
        #image_meta.addWidget(self.fallback)

        image_meta.addRow("Source", QLineEdit(""))

        self.style = QComboBox()
        self.style.addItem("-")
        self.style.addItem("2D")
        self.style.addItem("3D")
        self.style.addItem("Real")
        image_meta.addRow("Style", self.style)

        self.tag_view = TagViewWidget(repo=repo)
        layout.addWidget(self.tag_view, 0, 2, 2, 1)

        self.setLayout(layout)

        self.pack_data = None   # type: ResourcePack
        self.current_image = None

        self.update_image(0)

    def set_pack_data(self, data):
        self.pack_data = data
        self.current_image = None
        self.pack_list.clear()
        for i, image in enumerate(self.pack_data.images):
            new_item = QListWidgetItem(image.file)
            new_item.setData(Qt.UserRole, i)
            self.pack_list.addItem(new_item)
        self.update_image(0)

    def set_image_data(self, image_data):
        if isinstance(image_data, str):
            reader = QImageReader(image_data)
            reader.setAutoTransform(True)
            image = reader.read()
        else:
            image = QImage.fromData(image_data)
        image = image.scaled(self.image_width, self.image_height, aspectMode=Qt.KeepAspectRatio)
        self.pixmap = QPixmap.fromImage(image)
        self.label.setPixmap(self.pixmap)

    def update_image(self, image):
        if self.pack_data is None:
            self.label.setText("No Image Selected")
            self.current_image = None
            return
        if self.current_image is not None:
            self.tag_view.write_changes(self.pack_data.images[self.current_image])
        image_data = self.pack_data.images[image]   # type: ImageResource
        self.tag_view.update_resource(image_data)
        self.set_image_data(str(self.pack_data.path.parent / image_data.file))
        self.current_image = image
        self.pack_list.setCurrentRow(image)

    def next_image(self):
        self.update_image(min(self.current_image + 1, len(self.pack_data.images) - 1))

    def prev_image(self):
        self.update_image(max(0, self.current_image - 1))

    def _select_image(self, item):
        self.update_image(item) #.data(Qt.UserRole))

    def add_images(self, images: List[ImageResource]):
        old_count = len(self.pack_data.images)
        for image in images:
            image.file = str(Path(image.file).relative_to(self.pack_data.path.parent))
            self.pack_data.images.append(image)
        self.set_pack_data(self.pack_data)
        self.update_image(old_count)

    def duplicate_active_image(self):
        from copy import deepcopy
        copied = deepcopy(self.pack_data.images[self.current_image])
        self.pack_data.images.insert(self.current_image, copied)
        ci = self.current_image + 1
        self.set_pack_data(self.pack_data)
        self.update_image(ci)

    def remove_active_image(self):
        self.pack_data.images = self.pack_data.images[:self.current_image] + self.pack_data.images[self.current_image+1:]
        ci = self.current_image
        self.set_pack_data(self.pack_data)
        self.update_image(ci)
