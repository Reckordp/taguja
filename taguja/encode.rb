require 'zlib'

zcode = Zlib.deflate(File.read(ARGV[0]))
File.binwrite(ARGV[0].sub(/\.\w+$/, '.z'), zcode)