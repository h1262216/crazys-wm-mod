from PySide6 import QtGui
from tagger.main_widget import MainWidget
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog
from pathlib import Path
import json
from tagger.resource import read_tag_specs, guess_type_by_file_name

app = QApplication([])


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Tagger")

        self.repo = read_tag_specs(Path("../Resources/Data/ImageTypes.xml"))

        self.tagger = MainWidget(768, 768, self.repo)
        self.setup_menu_bar()

        # Set the central widget of the Window.
        self.setCentralWidget(self.tagger)

    def setup_menu_bar(self):
        file_menu = self.menuBar().addMenu("&File")
        new_action = QtGui.QAction('&New', self)
        new_action.setShortcut('Ctrl+N')
        new_action.setStatusTip('New pack')
        new_action.triggered.connect(self._new_pack_dlg)
        file_menu.addAction(new_action)

        load_action = QtGui.QAction('&Load', self)
        load_action.setShortcut('Ctrl+L')
        load_action.setStatusTip('Load pack')
        load_action.triggered.connect(self._load_pack_dlg)
        file_menu.addAction(load_action)

        save_action = QtGui.QAction('&Save', self)
        save_action.setShortcut('Ctrl+S')
        save_action.setStatusTip('Save current pack')
        save_action.triggered.connect(lambda: self._save_pack_dlg(save_as=False))
        file_menu.addAction(save_action)

        exit_action = QtGui.QAction('&Exit', self)
        exit_action.setShortcut('Ctrl+Q')
        exit_action.setStatusTip('Exit application')
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

        pack_menu = self.menuBar().addMenu("&Pack")
        action = QtGui.QAction('&Add Image', self)
        action.setShortcut('Ctrl+A')
        action.setStatusTip('Add image')
        action.triggered.connect(self._add_image_dlg)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Remove Image', self)
        action.setShortcut('Ctrl+F')
        action.setStatusTip('Removes an image from the pack')
        action.triggered.connect(self.tagger.remove_active_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Duplicate Image', self)
        action.setShortcut('Ctrl+D')
        action.setStatusTip('Duplicates the image and all associated tags')
        action.triggered.connect(self.tagger.duplicate_active_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('Add &Directory', self)
        action.setShortcut('Ctrl+Shift+A')
        action.setStatusTip('Add all images from directory')
        action.triggered.connect(self._add_dir_dlg)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Next Image', self)
        action.setShortcut('M')
        action.setStatusTip('Go to the next image')
        action.triggered.connect(self.tagger.next_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Previous Image', self)
        action.setShortcut('N')
        action.setStatusTip('Go to the previous image')
        action.triggered.connect(self.tagger.prev_image)
        pack_menu.addAction(action)

        stats_menu = self.menuBar().addMenu("&Stats")
        action = QtGui.QAction('&Missing', self)
        action.setStatusTip('Display the list of all image types without at least a single image')
        action.triggered.connect(self.tagger.list_missing_images)
        stats_menu.addAction(action)

    def _load_pack_dlg(self):
        pack_file, _ = QFileDialog.getOpenFileName(self, "Open Pack", ".", "Pack Files (images.xml)")
        if pack_file:
            from tagger.resource import load_image_pack
            self.tagger.set_pack_data(load_image_pack(pack_file))

    def _add_image_dlg(self):
        image_file, _ = QFileDialog.getOpenFileName(self, "Add Image", str(self.tagger.pack_data.path.parent), "Image (*.jpg *.jpeg *.png *.webp)")
        if image_file:
            from tagger.resource import ImageResource
            image = ImageResource(file=image_file, type=guess_type_by_file_name(image_file, self.repo), pregnant='', outfit='none')
            self.tagger.add_images([image])

    def _add_dir_dlg(self):
        directory = QFileDialog.getExistingDirectory(self, "Add Directory", str(self.tagger.pack_data.path.parent))
        if directory:
            from tagger.resource import ImageResource
            images = []
            for file in Path(directory).iterdir():
                if file.suffix not in [".png", ".jpg", ".jpeg", ".webp", ".gif"]:
                    continue
                image = ImageResource(file=file, type=guess_type_by_file_name(file, self.repo), pregnant='', outfit='none')
                images.append(image)
            self.tagger.add_images(sorted(images, key=lambda x: x.file))

    def _save_pack_dlg(self, save_as=True):
        if save_as:
            target, _ = QFileDialog.getSaveFileName(self, "Save Pack", str(self.tagger.pack_data.path), "Pack Files (images.xml)")
        else:
            target = self.tagger.pack_data.path
        from tagger.resource import save_image_pack
        save_image_pack(target, self.tagger.pack_data)

    def _new_pack_dlg(self):
        target, _ = QFileDialog.getSaveFileName(self, "New Pack", ".", "Pack Files (images.xml)")
        from tagger.resource import ResourcePack, save_image_pack
        pack = ResourcePack(target)
        self.tagger.set_pack_data(pack)
        save_image_pack(target, self.tagger.pack_data)


# Create a Qt widget, which will be our window.
window = MainWindow()
window.show()

# Start the event loop.
app.exec()
