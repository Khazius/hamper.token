#!/bin/bash
cleos -u https://mainnet.genereos.io set contract hamprtoken . hamper.wasm hamper.abi
cleos -u https://mainnet.genereos.io set account permission hamprtoken refund '{"threshold":1,"accounts":[{"permission":{"actor":"hamprtoken","permission":"eosio.code"},"weight":1}]}' active -p hamprtoken
cleos -u https://mainnet.genereos.io set action permission hamprtoken eosio.token transfer refund
cleos -u https://mainnet.genereos.io push action hamprtoken create '["hamprtoken","10000000 HAMPR"]' -p hamprtoken
