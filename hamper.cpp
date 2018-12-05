/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "hamper.hpp"

namespace eosio {

void token::create( account_name issuer,
                    asset        maximum_supply )
{
    require_auth( _self );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, sym.name() );
    auto existing = statstable.find( sym.name() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}


void token::issue( account_name to, asset quantity, string memo )
{
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto sym_name = sym.name();
    stats statstable( _self, sym_name );
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );

    if( to != st.issuer ) {
       SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
    }
}

void token::transfer( account_name from,
                      account_name to,
                      asset        quantity,
                      string       memo )
{
    eosio_assert( false, "Disabled - thanks for attending");
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );
    eosio_assert( is_account( to ), "to account does not exist");
    auto sym = quantity.symbol.name();
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );


    sub_balance( from, quantity );
    add_balance( to, quantity, from );
}

void token::empty( account_name owner, asset value ) {
  require_auth(_self);
  accounts from_acnts( _self, owner );
  const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
  from_acnts.erase( from );
}

void token::destroy(asset quantity) {
  require_auth(_self);
  auto sym = quantity.symbol;
  eosio_assert( sym.is_valid(), "invalid symbol name" );

  auto sym_name = sym.name();
  stats statstable( _self, sym_name );
  auto existing = statstable.find( sym_name );
  eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
  const auto& st = *existing;
  statstable.erase( st );
}

void token::sub_balance( account_name owner, asset value ) {
   accounts from_acnts( _self, owner );

   const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );


   if( from.balance.amount == value.amount ) {
      from_acnts.erase( from );
   } else {
      from_acnts.modify( from, owner, [&]( auto& a ) {
          a.balance -= value;
      });
   }
}

void token::add_balance( account_name owner, asset value, account_name ram_payer )
{
   accounts to_acnts( _self, owner );
   auto to = to_acnts.find( value.symbol.name() );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   } else {
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

void token::purchase( const transfer_args& t, name code ) {
    if(t.to == _self) {
      //eosio_assert( false, "Disabled - The ChrismEOS HAMPR Event has ended");
      eosio_assert( code == N(eosio.token), "only accept real EOS");
      eosio_assert( t.quantity.symbol == S(4,EOS), "only accept real EOS");
      eosio_assert( t.quantity.is_valid(), "invalid quantity in transfer" );
      eosio_assert( t.quantity.amount > 0, "zero quantity is disallowed in transfer");
      uint64_t payout = t.quantity.amount / 1'0000; //convert precision
      uint64_t refund = t.quantity.amount - (payout * 1'0000); //get the refund

      eosio_assert(payout > 0, "cannot purchase less than 1 HAMPR");
      asset quantity = asset(payout,S(0,HAMPR));

      action(
          permission_level{ _self, N(refund) },
          N(eosio.token), N(transfer),
          std::make_tuple(_self, t.from, asset(refund,S(4,EOS)), std::string("Refund fractional EOS from HAMPR purchase"))
      ).send();

      auto sym_name = quantity.symbol.name();
      stats statstable( _self, sym_name );
      auto existing = statstable.find( sym_name );
      eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
      const auto& st = *existing;

      eosio_assert( quantity.is_valid(), "invalid quantity" );
      eosio_assert( quantity.amount > 0, "must issue positive quantity" );

      eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
      eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

      statstable.modify( st, 0, [&]( auto& s ) {
         s.supply += quantity;
      });

      add_balance( st.issuer, quantity, st.issuer );

      if( t.from != st.issuer ) {
         SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, t.from, quantity, "Purchased HAMPR for the Australian ChristmEOS event" } );
      }
    }
  }

//intercept transfers
void token::apply( name contract, name action ) {

  if(contract == N(eosio.token) && action == N(transfer) ) {
     purchase( unpack_action_data<transfer_args>(), contract );
     return;
  }

  if( contract != _self ) return;
  auto& thiscontract = *this;
  switch( action ) {
     EOSIO_API( token, (create)(issue)(transfer)(destroy)(empty) )
  };
}

} /// namespace eosio

extern "C" {
   void apply( eosio::name receiver, eosio::name code, eosio::name action ) {
      eosio::token  c( receiver );
      c.apply( code, action );
   }
}
