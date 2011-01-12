# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# This script downloads and builds the third party dependencies for Windows.
# For each dependency do roughly the following steps:
#  - download and extract the source code
#  - patch the source and build environment as needed
#    - force the runtime library configuration to be /MD or /MDd
#    - force the building of static libraries
#    - fix bugs
#  - build both the Debug and Release versions of the library
#  - create an empty file in called <project>/.stamp to indicate success
#
# If the stamp file is present, then the script will not try to build the
# dependency again.  If you have trouble with a build, the best approach is to
# remove the source directory before rerunning the script.
#
# The current windows dependencies are:
#
#   =================  ===================================================
#   Name               Purpose
#   =================  ===================================================
#   Google Gtest       Unittests
#   Google Gmock       Unittests
#   =================  ===================================================
#
import urllib
import os.path
import tarfile
import subprocess
import os
import stat
import shutil
import zipfile
import xml.dom.minidom
import sys
from os.path import isfile, isdir, join as pj, exists
import hashlib
import optparse

# To save typing:
# pj = os.path.join
# wd = working directory
wd = os.path.dirname(os.path.abspath(__file__))

class XmlEditor(object):
  def __init__(self, path):
    print "[xml]", path
    self.path = path
    content = file(path, "rb").read()
    content = content.replace('<?xml version="1.0" encoding="shift_jis"?>', '')
    self.dom = xml.dom.minidom.parseString(content)
  def Save(self):
    os.chmod(self.path, stat.S_IWRITE)
    self.dom.writexml(file(self.path, "wb"))

def rm(path):
  if not exists(path):
    return
  if isdir(path):
    for sub_path in os.listdir(path):
      rm(pj(path, sub_path))
    print "[rm]", path
    while isdir(path):
      try:
        os.rmdir(path)
      except WindowsError, err:
        pass
  else:
    print "[rm]", path
    os.chmod(path, stat.S_IWRITE)
    for i in range(10):
      try:
        os.unlink(path)
      except:
        continue
      break

class Builder(object):  
  
  # True if the dependencies should be built with /MT or /MTd
  # False if the dependencies should be build with /MD or /MDd
  STATIC_RUNTIME = False
  
  MSBUILD_COMMAND = "msbuild"
  
  def __init__(self):
    pass
  
  def Fetch(self):
    raise NotImplemented()
  
  def Patch(self):
    raise NotImplemented()
  
  def Build(self):
    raise NotImplemented()
  
  def WriteStamp(self):
    file(pj(self.path, ".stamp"), "w").write("")
  
  def HasStamp(self):
    return isfile(pj(self.path, ".stamp"))

  #---- Helper Functions -------------------------------------------------------
  
  def Download(self, url, checksum):
    path = url.split("/")[-1]
    path = pj(wd, path)
    if isfile(path):
      if hashlib.md5(file(path, "rb").read()).hexdigest() == checksum:
        print "[  ok  ]", url
        return
    print "[download]", url
    urllib.urlretrieve(url, path)
    assert hashlib.md5(file(path, "rb").read()).hexdigest() == checksum

  def ExtractTarGz(self, path, out = None):
    if out is None: out = wd
    if not exists(path): path = pj(wd, path)
    print "[extract]", path
    tar = tarfile.open(path, mode="r:gz")
    tar.extractall(out)

  def ExtractZip(self, path, out = None):
    if out is None: out = wd
    if not exists(path): path = pj(wd, path)
    print "[extract]", path
    archive = zipfile.ZipFile(path, mode = "r")
    archive.extractall(out)

  def UpgradeVisualStudioFiles(self, root = None):  
    if root is not None: root = self.path
    for dirpath, dirnames, filenames in os.walk(root):
      for filename in filenames:
        filename = pj(dirpath, filename)
        if filename.endswith(".vcproj"):
          self.UpgradeVisualStudioFile(filename)
  
  def UpgradeVisualStudioFile(self, filename):
    if filename.endswith(".sln"):
      os.chmod(filename, stat.S_IWRITE)
      subprocess.call(["devenv", "/upgrade", filename])
      return
    try:
      xml = XmlEditor(filename)
    except:
      print "[WARNING] Cannot parse XML, upgrading anyway: " + filename
      subprocess.call(["devenv", "/upgrade", filename])
      return
    
    for el in xml.dom.getElementsByTagName("VisualStudioProject"):
      if float(el.getAttribute("Version")) >= 10.0:
        continue
      print "[upgrade]", filename
      os.chmod(filename, stat.S_IWRITE)
      subprocess.call(["devenv", "/upgrade", filename])

  def SetRuntimeLibrary(self, filename = None):
    if filename is None:
      filename = self.path
    if isdir(filename):
      for dirpath, dirnames, filenames in os.walk(filename):
        for filename in filenames:
          filename = pj(dirpath, filename)
          if filename.endswith(".vcproj") or filename.endswith(".vsprops"):
            print '[ setruntime ]', filename
            self.SetRuntimeLibrary(filename)
      return
    
    xml = XmlEditor(filename)
    for filter in xml.dom.getElementsByTagName("Tool"):
      if filter.getAttribute("Name") != u'VCCLCompilerTool':
        continue
      if self.STATIC_RUNTIME:
        if filter.getAttribute("RuntimeLibrary") == u"2":
          filter.setAttribute("RuntimeLibrary", "0")
        elif filter.getAttribute("RuntimeLibrary") == u"3":
          filter.setAttribute("RuntimeLibrary", "1")
      else:
        if filter.getAttribute("RuntimeLibrary") == u"0":
          filter.setAttribute("RuntimeLibrary", "2")
        elif filter.getAttribute("RuntimeLibrary") == u"1":
          filter.setAttribute("RuntimeLibrary", "3")
    xml.Save()

  def BuildSolution(self, path, target=None, configurations=None, args=None):
    cmd = [self.MSBUILD_COMMAND, path]
    if target is not None:
      cmd += ["/t:" + target]
    if args:
      cmd += args
    
    if configurations is None:
      configurations = ["Debug", "Release"]
    
    for configuration in configurations:
      subprocess.check_call(cmd + ["/p:Configuration=" + configuration],
        cwd = pj(wd))

class GtestBuilder(Builder):
  path = pj(wd, "gtest-1.5.0")
  vcproj = pj(path, "msvc", "gtest.vcproj")
  
  def Fetch(self):
    self.Download("http://googletest.googlecode.com/files/gtest-1.5.0.tar.gz",
      "7e27f5f3b79dd1ce9092e159cdbd0635")
  
  def Patch(self):
    rm(self.path)
    self.ExtractTarGz("gtest-1.5.0.tar.gz")
    self.UpgradeVisualStudioFiles(pj(self.path, "msvc"))
    
    xml = XmlEditor(self.vcproj)
    for el in xml.dom.getElementsByTagName("Tool"):
      if el.getAttribute("Name") != u"VCLibrarianTool":
        continue
      el.setAttribute("OutputFile", 
        el.getAttribute("OutputFile").replace("gtestd.lib", "gtest.lib"))
    xml.Save()
    
    self.SetRuntimeLibrary(pj(self.path, "msvc"))
  
  def Build(self):
    self.BuildSolution(pj(self.path, "msvc", "gtest.sln"))

class GmockBuilder(Builder):
  path = pj(wd, "gmock-1.5.0")
  
  def Fetch(self):
    self.Download("http://googlemock.googlecode.com/files/gmock-1.5.0.tar.gz",
      "d9e62a4702c300ae9c87284ca8da7fac")
  
  def Patch(self):
    rm(self.path)
    self.ExtractTarGz("gmock-1.5.0.tar.gz")
    self.UpgradeVisualStudioFiles(pj(self.path, "msvc"))
    self.SetRuntimeLibrary(pj(self.path, "msvc"))
  
  def Build(self):
    self.BuildSolution(pj(self.path, "msvc", "gmock.sln"))

builders = [
  GtestBuilder(),
  GmockBuilder(),
]

def main(argv):
  global builders

  parser = optparse.OptionParser()
  parser.add_option("--build", action="store_true")
  parser.add_option("--rebuild", action="store_true")
  parser.add_option("--clean", action="store_true")
  parser.add_option("--Debug", action="store_true")
  parser.add_option("--Release", action="store_true")
  parser.add_option("--msbuild", default="msbuild")
  parser.add_option("--static-runtime", action="store_true", default=False)
  parser.add_option("--dll-runtime", action="store_false",
    dest="static_runtime")
  (options, args) = parser.parse_args(argv)
  
  if os.name != 'nt':
    print >>sys.stderr, "This program should only be used to build the "\
      "Windows dependencies."
  
  Builder.STATIC_RUNTIME = options.static_runtime
  Builder.MSBUILD_COMMAND = options.msbuild
  
  if options.rebuild or options.clean:
    for builder in builders:
      rm(builder.path)
  if options.clean:
    return
  
  builders = [builder for builder in builders if not builder.HasStamp()]
  for builder in builders:
    builder.Fetch()
  for builder in builders:
    builder.Patch()
  for builder in builders:
    builder.Build()
    builder.WriteStamp()

if __name__ == "__main__":
  main(sys.argv)
