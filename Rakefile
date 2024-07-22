TARGET = %q(libnode.a)
SUMBER = ENV['NODE_PROJECT']
HEADER_MRUBY = ENV['MRUBY_HEADER']
HEADER_SHIPPED = ENV['SHIPPED_HEADER']
HEADER_JENNEIRE = ENV['JENNEIRE_HEADER']
HEADER_NUSA = ENV['NUSA_HEADER']
DEFINITION = <<~CMDLINE.split(/\n/)
  V8_DEPRECATION_WARNINGS
  V8_IMMINENT_DEPRECATION_WARNINGS
  _GLIBCXX_USE_CXX11_ABI=1
  NODE_OPENSSL_CONF_NAME=nodejs_conf
  NODE_OPENSSL_HAS_QUIC
  ICU_NO_USER_DATA_OVERRIDE
  WIN32
  _WIN32_WINNT=0x0602
  _CRT_NONSTDC_NO_DEPRECATE
  _HAS_EXCEPTIONS=0
  BUILDING_V8_SHARED=1
  BUILDING_UV_SHARED=1
  OPENSSL_NO_PINSHARED
  OPENSSL_THREADS
  OPENSSL_NO_ASM
  NODE_ARCH=\\"x64\\"
  NODE_WANT_INTERNALS=1
  V8_DEPRECATION_WARNINGS=1
  NODE_OPENSSL_SYSTEM_CERT_PATH=\\"\\"
  NODE_USE_NODE_CODE_CACHE=1
  HAVE_INSPECTOR=1
  HAVE_ETW=1
  FD_SETSIZE=1024
  NODE_PLATFORM=\\"win32\\"
  NOMINMAX
  _UNICODE=1
  NODE_USE_V8_PLATFORM=1
  NODE_V8_SHARED_RO_HEAP
  NODE_HAVE_I18N_SUPPORT=1
  HAVE_OPENSSL=1
  OPENSSL_API_COMPAT=0x10100000L
  BASE64_STATIC_DEFINE
  UCONFIG_NO_SERVICE=1
  U_ENABLE_DYLOAD=0
  U_STATIC_IMPLEMENTATION=1
  U_HAVE_STD_STRING=1
  UCONFIG_NO_BREAK_ITERATION=0
  NGHTTP2_STATICLIB
  NDEBUG
  OPENSSL_BUILDING_OPENSSL
  OPENSSL_SYS_WIN32
  WIN32_LEAN_AND_MEAN
  UNICODE
  _UNICODE
  _WINSOCK_DEPRECATED_NO_WARNINGS
  OPENSSL_PIC
  NGTCP2_STATICLIB
  NGHTTP3_STATICLIB
CMDLINE

CFLAGS = <<~CMDLINE.split(/\n/)
	-Wall
	-Wextra
	-Wno-unused-parameter
	-Werror=unused-result
	-O3
	-std=c++17
CMDLINE

HEADER = <<~CMDLINE.split(/\n/)
  deps/postject
  gen
  gen/src
  tools/msvs/genfiles
  deps/base64/base64/include
  deps/googletest/include
  deps/histogram/src
  deps/histogram/include
  deps/uvwasi/include
  deps/simdutf
  deps/ada
  deps/v8/include
  deps/icu-small/source/i18n
  deps/icu-small/source/common
  deps/zlib
  deps/llhttp/include
  deps/cares/include
  deps/uv/include
  deps/nghttp2/lib/includes
  deps/brotli/c/include
  deps/openssl/openssl/include
  deps/openssl/openssl/crypto
  deps/openssl/config/archs/VC-WIN64A/no-asm/include
  deps/openssl/config/archs/VC-WIN64A/no-asm
  deps/ngtcp2
  deps/ngtcp2/ngtcp2/lib/includes
  deps/ngtcp2/ngtcp2/crypto/includes
  deps/ngtcp2/ngtcp2/crypto
  deps/ngtcp2/nghttp3/lib/includes
CMDLINE

OBJS = <<~CMDLINE.split(/\n/)
  api/async_resource.o
  api/callback.o
  api/embed_helpers.o
  api/encoding.o
  api/environment.o
  api/exceptions.o
  api/hooks.o
  api/utils.o
  async_wrap.o
  base_object.o
  cares_wrap.o
  chat_manager.o
  cleanup_queue.o
  connect_wrap.o
  connection_wrap.o
  debug_utils.o
  env.o
  fs_event_wrap.o
  handle_wrap.o
  heap_utils.o
  histogram.o
  jendela.o
  js_native_api_v8.o
  js_stream.o
  json_utils.o
  js_udp_wrap.o
  module_wrap.o
  node.o
  node_api.o
  node_binding.o
  node_blob.o
  node_buffer.o
  node_builtins.o
  node_config.o
  node_constants.o
  node_contextify.o
  node_credentials.o
  node_dir.o
  node_env_var.o
  node_errors.o
  node_external_reference.o
  node_file.o
  node_http_parser.o
  node_http2.o
  node_i18n.o
  node_main_instance.o
  node_messaging.o
  node_metadata.o
  node_options.o
  node_os.o
  node_perf.o
  node_platform.o
  node_postmortem_metadata.o
  node_process_events.o
  node_process_methods.o
  node_process_object.o
  node_realm.o
  node_report.o
  node_report_module.o
  node_report_utils.o
  node_sea.o
  node_serdes.o
  node_shadow_realm.o
  node_snapshotable.o
  node_sockaddr.o
  node_stat_watcher.o
  node_symbols.o
  node_task_queue.o
  node_trace_events.o
  node_types.o
  node_url.o
  node_util.o
  node_v8.o
  node_wasi.o
  node_wasm_web_api.o
  node_watchdog.o
  node_worker.o
  node_zlib.o
  pipe_wrap.o
  process_wrap.o
  render_qrcode.o
  signal_wrap.o
  spawn_sync.o
  stream_base.o
  stream_pipe.o
  stream_wrap.o
  string_bytes.o
  string_decoder.o
  taguja.o
  tcp_wrap.o
  timers.o
  timer_wrap.o
  tracing/agent.o
  tracing/node_trace_buffer.o
  tracing/node_trace_writer.o
  tracing/trace_event.o
  tracing/traced_value.o
  tty_wrap.o
  udp_wrap.o
  util.o
  uv.o
  large_pages/node_large_page.o
  inspector_agent.o
  inspector_io.o
  inspector_profiler.o
  inspector_js_api.o
  inspector_socket.o
  inspector_socket_server.o
  inspector/main_thread_interface.o
  inspector/node_string.o
  inspector/runtime_agent.o
  inspector/tracing_agent.o
  inspector/worker_agent.o
  inspector/worker_inspector.o
  node_win32_etw_provider.o
  node_dtrace.o
  crypto/crypto_aes.o
  crypto/crypto_bio.o
  crypto/crypto_common.o
  crypto/crypto_dsa.o
  crypto/crypto_hkdf.o
  crypto/crypto_pbkdf2.o
  crypto/crypto_sig.o
  crypto/crypto_timing.o
  crypto/crypto_cipher.o
  crypto/crypto_context.o
  crypto/crypto_ec.o
  crypto/crypto_hmac.o
  crypto/crypto_random.o
  crypto/crypto_rsa.o
  crypto/crypto_spkac.o
  crypto/crypto_util.o
  crypto/crypto_clienthello.o
  crypto/crypto_dh.o
  crypto/crypto_hash.o
  crypto/crypto_keys.o
  crypto/crypto_keygen.o
  crypto/crypto_scrypt.o
  crypto/crypto_tls.o
  crypto/crypto_x509.o
  node_crypto.o
  msvs/pch/node_pch.o
  node_javascript.o
  src/node/inspector/protocol/Protocol.o
  src/node/inspector/protocol/NodeWorker.o
  src/node/inspector/protocol/NodeTracing.o
  src/node/inspector/protocol/NodeRuntime.o
  taguja_z.o
  penjawab_pesan_z.o
CMDLINE

OBJS_DLL = <<~CMDLINE.split(/\n/)
  taguja.o
  taguja_z.o
CMDLINE

GETOBJ = proc do |nama|
  File.join('build', nama)
end

GETSRC = proc do |hasil|
  hasil.sub(/build/, 'src').sub('.o', '.cc')
end

rule '.o' => GETSRC do |t|
  cmdheader = proc do |i|
    '-I ' + if i.match(/^[(?:deps)(?:tools)]/)
      File.join(SUMBER, i)
    elsif i.match(/^gen/)
      File.join(SUMBER, File.join('out', 'Release', 'obj', i))
    else
      raise "Tidak dapat menemukan lokasi " + i
    end
  end
  opt = DEFINITION.collect { |i| '-D' + i } .join(' ')
    .concat(OBJS_DLL.include?(File.basename(t.name)) ? ' -DCOMPILING_DLL ' : ' ')
    .concat(CFLAGS.join(' '))
    .concat(' -I src ')
    .concat(HEADER.collect(&cmdheader).join(' '))
    .concat(" -I #{HEADER_MRUBY} ")
    .concat(" -I #{HEADER_SHIPPED} ")
    .concat(" -I #{HEADER_JENNEIRE} ")
    .concat(" -I #{HEADER_NUSA}")
  tempat = File.dirname(t.name)
  sh('mkdir ' + tempat.gsub(/\//, "\\")) unless Dir.exist?(tempat)
  sh "g++ #{opt} -o #{t.name} -c #{t.prerequisites.first}"
  # sh "g++ #{opt} -o #{t.name} -c #{t.prerequisites.first} 2> err.txt"
end

file 'build/taguja_z.o' => 'taguja/taguja.js' do |t|
  Dir.chdir('taguja')
  sh 'ruby encode.rb taguja.js'
  sh "ld -r -b binary -o #{File.join('..', t.name)} taguja.z"
  Dir.chdir('..')
end

file 'build/penjawab_pesan_z.o' => 'taguja/penjawab_pesan.rb' do |t|
  Dir.chdir('taguja')
  sh 'ruby encode.rb penjawab_pesan.rb'
  sh "ld -r -b binary -o #{File.join('..', t.name)} penjawab_pesan.z"
  Dir.chdir('..')
end

file 'taguja.dll' => OBJS_DLL.collect(&GETOBJ) do |t|
  optlib = '-Wl,--out-implib,' + t.name + '.a'
  sh "g++ -shared -o #{t.name} #{t.prerequisites.join(' ')} #{optlib}"
end

for_static_library = OBJS.collect(&GETOBJ)
# for_static_library.push('taguja.dll')
file TARGET => for_static_library do |t|
  sh 'ar rcs -o ' + t.name + ' ' + t.prerequisites.join(' ')
  Rake::Task['build/node_main.o'].invoke('src/node_main.cc')
  sh 'copy build\node_main.o .'
end

Dir.mkdir('build') unless Dir.exist?('build')

task default: TARGET