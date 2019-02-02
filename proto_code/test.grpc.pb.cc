// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: test.proto

#include "test.pb.h"
#include "test.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace rpcpb {

static const char* Test_method_names[] = {
  "/rpcpb.Test/HowAreYou",
};

std::unique_ptr< Test::Stub> Test::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Test::Stub> stub(new Test::Stub(channel));
  return stub;
}

Test::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_HowAreYou_(Test_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Test::Stub::HowAreYou(::grpc::ClientContext* context, const ::rpcpb::TestReq& request, ::rpcpb::TestRsp* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_HowAreYou_, context, request, response);
}

void Test::Stub::experimental_async::HowAreYou(::grpc::ClientContext* context, const ::rpcpb::TestReq* request, ::rpcpb::TestRsp* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_HowAreYou_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::rpcpb::TestRsp>* Test::Stub::AsyncHowAreYouRaw(::grpc::ClientContext* context, const ::rpcpb::TestReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::rpcpb::TestRsp>::Create(channel_.get(), cq, rpcmethod_HowAreYou_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::rpcpb::TestRsp>* Test::Stub::PrepareAsyncHowAreYouRaw(::grpc::ClientContext* context, const ::rpcpb::TestReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::rpcpb::TestRsp>::Create(channel_.get(), cq, rpcmethod_HowAreYou_, context, request, false);
}

Test::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Test_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Test::Service, ::rpcpb::TestReq, ::rpcpb::TestRsp>(
          std::mem_fn(&Test::Service::HowAreYou), this)));
}

Test::Service::~Service() {
}

::grpc::Status Test::Service::HowAreYou(::grpc::ServerContext* context, const ::rpcpb::TestReq* request, ::rpcpb::TestRsp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace rpcpb
