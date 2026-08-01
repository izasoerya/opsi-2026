#pragma once

// =====================================================================
//  Kredensial ASLI ada di sini. File ini SUDAH di-ignore lewat
//  .gitignore, jadi jangan lupa dicek `git status` sebelum commit
//  supaya file ini beneran gak ke-track.
// =====================================================================

// --- WiFi ---

// --- WireGuard ---
// WG_LOCAL_IP dipakai langsung sebagai argumen IPAddress(...), makanya
// tanpa tanda kutip dan dipisah koma.
#define WG_LOCAL_IP            0,0,0,0
#define WG_PRIVATE_KEY         "WG_PRIVATE_KEY"
#define WG_ENDPOINT_ADDRESS    "Server WG"
#define WG_ENDPOINT_PUBLIC_KEY "endpoint public key"
#define WG_ENDPOINT_PORT       endpoint port

// --- OTA (ElegantOTA) ---
#define OTA_USERNAME "usernameOTA"
#define OTA_PASSWORD "PasswordOTA"