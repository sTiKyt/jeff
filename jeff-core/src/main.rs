//! Jeff Core.

mod hyper_router;
mod logic;
mod model;
mod setup;

use std::sync::Arc;
use tokio::sync::Mutex;

use model::{Db, DaemonsHolder};

pub async fn shutdown() {
  tokio::signal::ctrl_c()
    .await
    .expect("Failed to set Ctrl+C combination as shutdown.");
}

#[tokio::main]
pub async fn main() {
  let cfg = setup::get_config();
  let manager = bb8_postgres::PostgresConnectionManager::new_from_stringlike(
                    cfg.pg.clone(),
                    tokio_postgres::NoTls)
                  .unwrap();
  let pool = bb8::Pool::builder().max_size(15).build(manager).await.unwrap();
  let db = Db::new(pool);
  let service = hyper::service::make_service_fn(move |conn: &hyper::server::conn::AddrStream| {
    let db = db.clone();
    let admin_key = cfg.admin_key.clone();
    let addr = conn.remote_addr();
    let daemons = DaemonsHolder::load(&cfg.daemons);
    let daemons_mutex = Arc::new(Mutex::new(daemons));
    let service = hyper::service::service_fn(move |req| {
      hyper_router::router(req, db.clone(), admin_key.clone(), addr, daemons_mutex.clone())
    });
    async move { Ok::<_, std::convert::Infallible>(service) }
  });
  let server = hyper::Server::bind(&cfg.hyper_addr).serve(service);
  println!("Server listening at http://{}", cfg.hyper_addr);
  let finisher = server.with_graceful_shutdown(shutdown());
  match finisher.await {
    Err(e) => eprintln!("Server Error: {}", e),
    _ => println!("\nServer successfully shutted down."),
  }
}
